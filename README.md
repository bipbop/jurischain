# proof-of-work

# API

## Navegador Web
```html
  <link href="./style.css" rel="stylesheet" type="text/css" />
  <script>
    /* Configuração do Desafio */
    document.robotCaptcha = {
      seed: 'TrueRandomValue',
      difficulty: 10,
    };
    /* gera um evento quando resolvido */
    document.addEventListener('robotCaptcha', ({ detail }) => console.log(detail));
  </script>
  /* Elemento */
  <div id="robot-captcha"></div>
  <script src="./sha3.js"></script>
```

## C
```c
void pow_gen(pow_ctx_t *challenge, uint8_t difficulty, const void *seed, size_t inlen);

- Gera um novo challenge com um grau de complexidade e uma semente.
```
```c
int pow_verify(pow_ctx_t *challenge)

- Recebe um ponteiro com challenge e verifica se ela resolve o desafio, retornando 1 no caso de resolver ou 0 no caso de não resolver.
```
```c
int pow_try(pow_ctx_t *challenge)

- Recebe um ponteiro com challenge tenta resolver o desafio, retornando 1 no caso de resolver ou 0 no caso de não resolver.
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
