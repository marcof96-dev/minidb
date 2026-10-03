#!/usr/bin/env bash
# test_repl.sh - uso: ./test_repl.sh [binario] [BUF_SIZE]
BIN=${1:-./minidb}
BUF_SIZE=${2:-16}

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

# esegue il comando con un timeout di 3 secondi; se exec fallisce esce con 127
t() { perl -e 'alarm 3; exec @ARGV; print STDERR "exec fallita: $!\n"; exit 127' "$@"; }

# genera N caratteri 'a'
rep() { head -c "$1" /dev/zero | tr '\0' 'a'; }

# 1. Riga normale: echo
out=$(printf 'ciao\n' | t "$BIN" 2>&1); code=$?
check "echo riga normale" 0 "ciao" "Errore"

# 2. Piu' righe
out=$(printf 'uno\ndue\ntre\n' | t "$BIN" 2>&1); code=$?
check "echo piu' righe" 0 "tre" "Errore"

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
out=$(printf '\0\nciao\n' | t "$BIN" 2>&1); code=$?
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

# 9. Riga valida seguita da riga troppo lunga: la prima va in echo, poi errore
out=$( { printf 'ok\n'; rep $((BUF_SIZE * 2)); printf '\n'; } | t "$BIN" 2>&1); code=$?
check "riga ok poi riga lunga: errore" 0 "troppo grande" ""
check "riga ok poi riga lunga: echo di ok" 0 "ok" ""

# 10. Ultima riga senza \n (EOF a meta' riga): deve essere accettata
out=$(printf 'ciao' | t "$BIN" 2>&1); code=$?
check "ultima riga senza \\n" 0 "ciao" "troppo grande"

# 11. Dopo una riga troppo lunga il resto va scartato e la riga successiva e' letta bene
#     (verifica che il ciclo che svuota stdin funzioni)
out=$( { rep $((BUF_SIZE * 3)); printf '\nciao\n'; } | t "$BIN" 2>&1); code=$?
check "ripresa dopo riga lunga" 0 "ciao" "Errore"

# 12. Il messaggio d'errore riporta il limite corretto (BUF_SIZE-2)
out=$( { rep $((BUF_SIZE * 2)); printf '\n'; } | t "$BIN" 2>&1); code=$?
check "messaggio con limite $((BUF_SIZE - 2))" 0 "$((BUF_SIZE - 2)) caratteri" ""

echo
echo "Risultato: $pass passati, $fail falliti"
exit $(( fail > 0 ))