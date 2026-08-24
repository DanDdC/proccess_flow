# ProcessFlow

Interpretador de fluxos de processos em C (POSIX): cadastra tarefas, executa programas reais do sistema em modos sequencial, paralelo e pipeline, com redirecionamento de E/S, diretório de trabalho, jobs em background e modo workflow por script.

## Funcionalidades

| Comando | O que faz |
|---|---|
| `task <nome> <programa> [args...]` | cadastra uma tarefa |
| `run <nome>` | executa a tarefa e espera |
| `run sequential <n1> <n2> ...` | executa todas, uma depois da outra, na ordem |
| `run parallel <n1> <n2> ...` | lança todas ao mesmo tempo e colhe no fim |
| `run pipe <n1> <n2> ...` | conecta o stdout de um ao stdin do próximo |
| `input <nome> <arquivo>` | tarefa lê do arquivo em vez do teclado |
| `output <nome> <arquivo>` | grava stdout no arquivo (sobrescreve) |
| `append <nome> <arquivo>` | grava stdout no arquivo (adiciona ao fim) |
| `workdir <caminho>` | muda o diretório das próximas execuções |
| `start <nome>` | roda em background e devolve o prompt na hora |
| `jobs` | lista os jobs ativos (`[id] pid nome`) |
| `wait <id>` | espera um job específico terminar |
| `exit` | encerra o interpretador |

Situações de erro cobertas: fatais (argumentos inválidos na linha do programa; script de workflow que não abre) encerram com código ≠ 0; não-fatais (tarefa/programa/arquivo/job/diretório inexistentes) imprimem mensagem em stderr e seguem. Processos filhos que terminam com código ≠ 0 geram aviso.

## Como compilar

```bash
make clean
make
```

Requisitos: `gcc` com suporte a C99 e bibliotecas POSIX (testado em Ubuntu/WSL).

## Como executar

```bash
./processflow              # modo interativo
./processflow workflow.pf  # modo workflow: lê comandos do arquivo
```

Exemplo de sessão:

```
processflow> task ola /bin/echo OLA MUNDO
processflow> run ola
OLA MUNDO
processflow> start ola
[1] 12345
processflow> jobs
[1] 12345 ola
processflow> exit
```

Um fluxo completo de exemplo está em `workflow.pf`.

## Como testar

```bash
make test
```

Roda a suíte própria (`run_tests.sh`), autocontida: cria os próprios casos em `/tmp` e verifica os comportamentos centrais do interpretador (erros não-fatais, workflow fatal, ordem do sequential, timing do paralelo, pipeline, redirecionamentos combinados, append, workdir, aviso de código de saída ≠ 0, comentários e ciclo de jobs).

## Estrutura do projeto

| Arquivo | Responsabilidade |
|---|---|
| `main.c` | loop principal (interativo/workflow) e dispatcher de comandos |
| `task.h/.c` | cadastro e busca de tarefas (vetor fixo) |
| `exec.h/.c` | fork/execvp/waitpid: rodar, sequencial, paralelo, pipeline e lançamento p/ jobs |
| `redirect.h/.c` | configuração e aplicação de redirecionamento de E/S no filho |
| `job.h/.c` | tabela de jobs em background, coleta de zumbis, start/jobs/wait |
| `workflow.pf` | exemplo de fluxo |
| `run_tests.sh` | suíte própria de verificação (alvo `test`) |
| `Makefile` | targets `all`, `clean`, `test` |

## Decisões técnicas principais

- **Vetores fixos, sem alocação dinâmica**: limites claros (`MAX_TAREFAS=32`, `MAX_ARGS=32`), reduzindo o risco de memory leak;
- **`execvp`**: busca o programa no PATH, então `/bin/echo` e `echo` funcionam igualmente;
- **Filho usa `_exit(127)`** quando `execvp` falha, para não despejar buffers duplicados;
- **Pipeline aborta a cadeia inteira se qualquer elo não existe** (um furo quebraria o fluxo); já o `sequential` apenas pula o elo faltante e continua;
- **Disciplina de fechamento de pontas** do pipe no pai e em todos os filhos, evitando deadlock por descritor aberto;
- **Zumbis**: `waitpid(-1, ..., WNOHANG)` no topo de cada iteração do loop recolhe filhos mortos em background;
- **Ids de job nunca são reaproveitados**, evitando ambiguidade no `wait`.

## Limitações conhecidas

- Nos modos `parallel` e `pipe`, o aviso de código de saída ≠ 0 identifica o processo pelo PID (não pelo nome da tarefa);
- O `pipe` ignora redirecionamentos configurados nas tarefas intermediárias (o cano sobrepõe os arquivos);
