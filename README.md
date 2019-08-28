# proof-of-work

# API
```c
uint8_t* pow_initrand(const char* seed)

- Inicializa a seed de aleatoriedade do programa, não é necessário usar o valor de retorno, porém é importante que seja a primeira função a ser chamada.
```
```c
uint8_t* pow_gen(uint8_t difficulty, uint8_t* challenge, uint seedSize)

- Gera um novo challenge e retorna um uint8_t* de 33 bytes onde os 32 primeiros são o desafio e o último a dificuldade.
```
```c
int pow_verify(uint8_t challenge[33], uint8_t answer[32])

- Recebe um ponteiro com challenge[32] + dificuldade[1] e uma resposta[32] e verifica se ela resolve o desafio, retornando 1 no caso de resolver ou 0 no caso de não resolver.
```
```c
int pow_try(uint8_t* challenge, uint8_t* answer)

- Gera uma tentativa de resposta para o desafio aleatoriamente, retornando 1 caso consiga resolver (com o conteúdo da resposta no buffer <answer>) ou 0 caso não consiga (<answer> = NULL) 
```

# Estatísticas
Para gerar o gráfico de média de tentativas/dificuldade de desafio, faça o seguinte:

```bash
make all
virtualenv .env/
pip install -r requirements.txt
python3 genstats.py
```
Os resultados estarão no diretório `stats/`.
