#!/usr/bin/env bash
# test_repl.sh - uso: ./test_repl.sh [binario] [BUF_SIZE]
BIN=${1:-./minidb}
BUF_SIZE=${2:-1024}

pass=0; fail=0

# check NOME EXIT_ATTESO PATTERN_PRESENTE PATTERN_ASSENTE
# l'output e il codice di uscita vanno messi in $out e $code prima della chiamata
check() {
    local name=$1 exp_code=$2 present=$3 absent=$4 ok=1
    [[ $code -ne $exp_code ]] && ok=0
    [[ -n $present ]] && ! grep -qF -- "$present" <<<"$out" && ok=0
    [[ -n $absent  ]] &&   grep -qF -- "$absent"  <<<"$out" && ok=0
    if (( ok )); then
        echo "PASS  $name"; ((pass++))
    else
        echo "FAIL  $name (exit=$code, atteso=$exp_code)"
        echo "      output: $(printf '%s' "$out" | head -c 200)"
        ((fail++))
    fi
}

# se il binario non esiste o non e' eseguibile ci fermiamo subito con un messaggio chiaro
if [[ ! -x $BIN ]]; then
    echo "Errore: '$BIN' non esiste o non e' eseguibile (compilalo, o passa il percorso: ./test_repl.sh ./nome_binario)"
    exit 2
fi

# percorso assoluto del binario: i test girano in un'altra cartella
BIN="$(cd "$(dirname "$BIN")" && pwd)/$(basename "$BIN")"

# ogni test gira in una cartella temporanea sua, cosi' il minidb.aof
# di un test non contamina gli altri; tutto viene cancellato all'uscita
TMP_ROOT=$(mktemp -d)
trap 'rm -rf "$TMP_ROOT"' EXIT

# esegue il comando nella cartella $1 con un timeout di 3 secondi; se exec fallisce esce con 127
run_in() { local dir=$1; shift; (cd "$dir" && perl -e 'alarm 3; exec @ARGV; print STDERR "exec fallita: $!\n"; exit 127' "$@"); }

# t: esegue in una cartella nuova e vuota (nessun AOF dai test precedenti)
t() { run_in "$(mktemp -d "$TMP_ROOT/test.XXXXXX")" "$@"; }

# per i test di persistenza: new_work crea la cartella (va chiamata fuori da $(...),
# altrimenti WORK resta nella subshell), t_same esegue sempre li'
new_work() { WORK=$(mktemp -d "$TMP_ROOT/aof.XXXXXX"); }
t_same() { run_in "$WORK" "$@"; }

# genera N caratteri 'a'
rep() { head -c "$1" /dev/zero | tr '\0' 'a'; }

# 1. Comando normale: SET poi GET restituisce il valore
out=$(printf 'SET a ciao\nGET a\n' | t "$BIN" 2>&1); code=$?
check "SET poi GET" 0 "ciao" "Error"

# 2. Piu' righe: ogni comando vede l'effetto dei precedenti
out=$(printf 'SET a uno\nSET b due\nSET c tre\nGET c\n' | t "$BIN" 2>&1); code=$?
check "piu' righe" 0 "tre" "Error"

# 3. EOF immediato (Ctrl-D subito)
out=$(printf '' | t "$BIN" 2>&1); code=$?
check "EOF immediato" 0 "> " "Errore"

# 4. Riga vuota (solo invio)
out=$(printf '\n' | t "$BIN" 2>&1); code=$?
check "riga vuota (solo \\n)" 0 "> " "Errore"

# 5. Lunghezza nulla: un byte NUL all'inizio fa avere strlen == 0
#    Il programma stampa l'errore ma NON esce (continue) -> codice 0
out=$(printf '\0\n' | t "$BIN" 2>&1); code=$?
check "lunghezza nulla (NUL): errore" 0 "lunghezza nulla" ""

# 5b. Dopo l'errore di lunghezza nulla il REPL deve continuare a leggere
out=$(printf '\0\nSET a ciao\nGET a\n' | t "$BIN" 2>&1); code=$?
check "lunghezza nulla (NUL): ripresa" 0 "ciao" ""

# 6. Limite: BUF_SIZE-2 caratteri + \n = esattamente il massimo accettato
out=$( { rep $((BUF_SIZE - 2)); printf '\n'; } | t "$BIN" 2>&1); code=$?
check "limite massimo ($((BUF_SIZE - 2)) car.)" 0 "" "troppo grande"

# 7. Appena sopra il limite: errore stampato, ma il programma NON esce (continue) -> codice 0
out=$( { rep $((BUF_SIZE - 1)); printf '\n'; } | t "$BIN" 2>&1); code=$?
check "appena sopra il limite ($((BUF_SIZE - 1)) car.)" 0 "troppo grande" ""

# 8. Riga molto lunga: stesso comportamento, codice 0
out=$( { rep $((BUF_SIZE * 5)); printf '\n'; } | t "$BIN" 2>&1); code=$?
check "riga molto lunga" 0 "troppo grande" ""

# 9. Riga valida seguita da riga troppo lunga: la prima viene eseguita, poi errore
out=$( { printf 'SET a valore1\nGET a\n'; rep $((BUF_SIZE * 2)); printf '\n'; } | t "$BIN" 2>&1); code=$?
check "riga ok poi riga lunga: errore" 0 "troppo grande" ""
check "riga ok poi riga lunga: comando eseguito" 0 "valore1" ""

# 10. Ultima riga senza \n (EOF a meta' riga): deve essere accettata
out=$(printf 'SET a ciao\nGET a' | t "$BIN" 2>&1); code=$?
check "ultima riga senza \\n" 0 "ciao" "troppo grande"

# 11. Dopo una riga troppo lunga il resto va scartato e la riga successiva e' letta bene
#     (verifica che il ciclo che svuota stdin funzioni)
out=$( { rep $((BUF_SIZE * 3)); printf '\nSET a ciao\nGET a\n'; } | t "$BIN" 2>&1); code=$?
check "ripresa dopo riga lunga" 0 "ciao" "Error"

# 12. Il messaggio d'errore riporta il limite corretto (BUF_SIZE-2)
out=$( { rep $((BUF_SIZE * 2)); printf '\n'; } | t "$BIN" 2>&1); code=$?
check "messaggio con limite $((BUF_SIZE - 2))" 0 "$((BUF_SIZE - 2)) caratteri" ""

# 13. Nomi dei comandi case insensitive
out=$(printf 'set a ciao\ngEt a\n' | t "$BIN" 2>&1); code=$?
check "comandi case insensitive" 0 "ciao" "Error"

# 14. Le chiavi invece sono case sensitive
out=$(printf 'SET Nome ciao\nGET nome\n' | t "$BIN" 2>&1); code=$?
check "chiavi case sensitive" 0 "(nil)" "ciao"

# 15. Update: SET su una chiave esistente sostituisce il valore
out=$(printf 'SET a vecchio\nSET a nuovo\nGET a\n' | t "$BIN" 2>&1); code=$?
check "update di una chiave" 0 "nuovo" "vecchio"

# 16. DEL: (1) la prima volta, (0) la seconda, poi GET da' (nil)
out=$(printf 'SET a 1\nDEL a\nDEL a\nGET a\n' | t "$BIN" 2>&1); code=$?
check "DEL di una chiave presente" 0 "(1)" ""
check "DEL di una chiave gia' cancellata" 0 "(0)" ""
check "GET dopo DEL" 0 "(nil)" ""

# 17. Comando sconosciuto: errore, ma il REPL continua
out=$(printf 'PIPPO a\nSET a ciao\nGET a\n' | t "$BIN" 2>&1); code=$?
check "comando sconosciuto: errore" 0 "unknown command" ""
check "comando sconosciuto: ripresa" 0 "ciao" ""

# 18. Numero di argomenti sbagliato: errore e la tabella non cambia
out=$(printf 'SET a\nGET a\n' | t "$BIN" 2>&1); code=$?
check "argomenti sbagliati: errore" 0 "wrong number" ""
check "argomenti sbagliati: niente inserito" 0 "(nil)" ""

# 19. Troppi token: errore del REPL, il comando non viene eseguito
out=$(printf 'SET a b c\nGET a\n' | t "$BIN" 2>&1); code=$?
check "troppi token" 0 "Too many tokens" "OK"

# 20. Riga di soli spazi: viene saltata senza errori
out=$(printf '   \t  \n' | t "$BIN" 2>&1); code=$?
check "riga di soli spazi" 0 "> " "Error"

# --- Persistenza (AOF) ---

# 21. Riavvio: i dati della prima esecuzione ci sono nella seconda
new_work
out=$(printf 'SET a 1\nSET b 2\nDEL a\n' | t_same "$BIN" 2>&1)
out=$(printf 'GET b\nGET a\n' | t_same "$BIN" 2>&1); code=$?
check "riavvio: SET sopravvive" 0 "> 2" ""
check "riavvio: DEL sopravvive" 0 "(nil)" ""

# 22. Il file contiene esattamente i comandi che hanno modificato la tabella
out=$(cat "$WORK/minidb.aof"); code=0
expected=$'SET a 1\nSET b 2\nDEL a'
if [[ $out == "$expected" ]]; then
    echo "PASS  contenuto dell'AOF"; ((pass++))
else
    echo "FAIL  contenuto dell'AOF"
    echo "      file: $(printf '%s' "$out" | head -c 200)"
    ((fail++))
fi

# 23. Comandi che non modificano la tabella non vengono scritti
new_work
out=$(printf 'GET a\nDEL x\nPIPPO\nSET a\n' | t_same "$BIN" 2>&1)
size=$(wc -c < "$WORK/minidb.aof" | tr -d ' ')
out="size=$size"; code=0
check "letture ed errori non scritti nell'AOF" 0 "size=0" ""

# 24. Riga troncata (crash simulato): viene rimossa e il file resta usabile
new_work
out=$(printf 'SET a 1\n' | t_same "$BIN" 2>&1)
printf 'SET c 3' >> "$WORK/minidb.aof"
out=$(printf 'GET c\nSET d 4\n' | t_same "$BIN" 2>&1); code=$?
check "riga troncata: segnalata" 0 "Truncated" ""
check "riga troncata: non eseguita" 0 "(nil)" ""
out=$(cat "$WORK/minidb.aof"); code=0
expected=$'SET a 1\nSET d 4'
if [[ $out == "$expected" ]]; then
    echo "PASS  riga troncata: file riparato"; ((pass++))
else
    echo "FAIL  riga troncata: file riparato"
    echo "      file: $(printf '%s' "$out" | head -c 200)"
    ((fail++))
fi

echo
echo "Risultato: $pass passati, $fail falliti"
exit $(( fail > 0 ))