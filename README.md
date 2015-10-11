# Roteador
Trabalho de Redes

## Compilando
Acesse a pasta do código fonte via terminal, execute o ```make```:
```
$ make
gcc *.c -pthread -lncurses -o Router -Wall -g
$
```

## Executando
Certifique-se que as configurações dos arquivos ```roteador.config``` e ```enlaces.config``` estão corretas e execute o programa da seguinte maneira:
```
$ ./Router --ID=1
```
ou
```
$ ./Router -i1
```

### Argumentos adicionais
Opções para incluir na execução do programa ```--option=<value>``` ou ```-i<value>```:
 * ```--usage```: Não recebe valor, resume como utilizar o programa (parâmetros aceitos)
 * ```--version | -V```: Não recebe valor, imprime a versão do programa
 * ```--help | -?```: Não recebe valor. Ajuda do programa
 * ```--ID | -i```: Recebe a ID do roteador a ser inicializado. Deve estar presente no roteador.config (OBRIGATÓRIO)
 * ```--timeout | -t```: Tempo máximo de espera (em ms) pela confirmação de recebimento de uma mensagem (Default: 3000 ms)
 * ```--attempts | -r```: Número de reenvios de mensagens não confirmadas (após isso aborta) (Default: 3)
 * ```--ifdelay | -d```: Tempo (em μs) entre o envio de uma mensagem e outra (Default: 175 μs)
 * ```--sleep | -s```: Tempo de espera (em ms) entre um processamento da fila de transmissão e outro. (Default: 200 ms)
