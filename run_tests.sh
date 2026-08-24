#!/bin/bash
#suíte de verificação do ProcessFlow — autocontida: cria os próprios casos em /tmp
cd "$(dirname "$0")" || exit 1
[ -x ./processflow ] || { echo "compile antes: make"; exit 1; }

P=0; F=0
chk(){ if [ "$2" = "$3" ]; then P=$((P+1)); echo "PASS: $1"; else F=$((F+1)); echo "FAIL: $1 (obteve '$2', esperava '$3')"; fi }

TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT

#1 erros não-fatais: duplicata, uso, desconhecido, tarefa inexistente
N=$(printf 'task dup /bin/echo x\ntask dup /bin/echo y\ntask\nfoo\nrun nada\nexit\n' | ./processflow 2>&1 >/dev/null | wc -l)
chk "erros nao-fatais (dup/uso/desconhecido/inexistente)" "$N" "4"

#2 run simples (interativo: sem eco de linhas)
O=$(printf 'task ok /bin/echo funcionou\nrun ok\nexit\n' | ./processflow 2>/dev/null | tr ' ' '\n' | grep -cx funcionou)
chk "run simples executa" "$O" "1"

#3 workflow fatal: arquivo inexistente
./processflow "$TMP/fantasma.txt" >/dev/null 2>&1
chk "workflow inexistente é fatal (rc=1)" "$?" "1"

#4 programa inexistente
printf 'task ruim /nao/existe\nrun ruim\nexit\n' | ./processflow >/dev/null 2>"$TMP/e.txt"
grep -q "programa não encontrado" "$TMP/e.txt"
chk "programa inexistente reportado" "$?" "0"

#5 sequential preserva a ordem
S=$(printf 'task a /bin/echo AAA\ntask b /bin/echo BBB\nrun sequential a b\nexit\n' | ./processflow 2>/dev/null | tr ' ' '\n' | grep -x 'AAA\|BBB' | tr '\n' ' ')
chk "sequential em ordem" "$S" "AAA BBB "

#6 parallel roda no tempo de UM sleep (~1s)
T0=$(date +%s%N)
printf 'task s1 /bin/sleep 1\ntask s2 /bin/sleep 1\ntask s3 /bin/sleep 1\nrun parallel s1 s2 s3\nexit\n' | ./processflow >/dev/null 2>&1
T1=$(date +%s%N); MS=$(( (T1-T0)/1000000 ))
[ $MS -lt 2000 ]; chk "parallel ~1s (${MS}ms)" "$?" "0"

#7 pipeline conecta os estágios
G=$(printf 'task g /usr/bin/seq 5 -1 1\ntask o /usr/bin/sort\ntask c /bin/cat\nrun pipe g o c\nexit\n' | ./processflow 2>/dev/null | tr ' ' '\n' | grep -x '[1-5]' | tr '\n' ' ')
chk "pipe ordena 5..1" "$G" "1 2 3 4 5 "

#8 redirecionamento combinado input+output
printf 'zebra\nmacaco\n' > "$TMP/in.txt"
printf 'task o /usr/bin/sort\ninput o %s\noutput o %s\nrun o\nexit\n' "$TMP/in.txt" "$TMP/out.txt" | ./processflow >/dev/null 2>&1
R=$(tr '\n' ' ' < "$TMP/out.txt")
chk "input+output" "$R" "macaco zebra "

#9 append acumula
printf 'task e /bin/echo linha\nappend e %s\nrun e\nrun e\nexit\n' "$TMP/ap.txt" | ./processflow >/dev/null 2>&1
chk "append acumula 2 linhas" "$(wc -l < "$TMP/ap.txt")" "2"

#10 workdir muda o diretório dos filhos
W=$(printf 'workdir /\ntask l /bin/ls\nrun l\nexit\n' | ./processflow 2>/dev/null | grep -c boot)
chk "workdir muda diretorio" "$W" "1"

#11 aviso de código de saída != 0
A=$(printf 'task f /bin/false\nrun f\nexit\n' | ./processflow 2>&1 >/dev/null | grep -c "aviso: 'f' terminou com código 1")
chk "aviso de exit code != 0" "$A" "1"

#12 comentários com # são ignorados no workflow
C=$(printf '#isto e comentario\ntask k /bin/echo kkk\nrun k\nexit\n' | ./processflow 2>&1 >/dev/null | grep -c desconhecido)
chk "comentario # sem ruido" "$C" "0"

#13 jobs: start devolve id, wait colhe, jobs esvazia
J=$(printf 'task s /bin/sleep 2\nstart s\nwait 1\njobs\nexit\n' | ./processflow 2>/dev/null | tr ' ' '\n' | grep -cx '\[1\]')
chk "start imprime jobId" "$J" "1"
L=$(printf 'wait 99\nexit\n' | ./processflow 2>&1 >/dev/null | grep -c "job 99 não existe")
chk "job inexistente nao-fatal" "$L" "1"

echo "-----------------------------------"
echo "suite: $P PASS, $F FAIL"
[ $F -eq 0 ]
