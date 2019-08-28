#include <stdint.h>
#include <stdio.h>

#include "emscripten.h"
#include "juschain.h"
#include "string.h"

pow_ctx_t pow_i;
uint8_t trys = 0;
size_t len = 32;

char *loading = "<div class='juschain-captcha__marker'><div "
                "class='juschain-captcha__checkbox--loading'></div></div>"
                "<div class='juschain-captcha__text'>Aguarde enquanto validamos "
                "seu acesso.</span></div>";

char *completed = "<div class='juschain-captcha__marker'><div "
                  "class='juschain-captcha__checkbox--completed'></div></div>"
                  "<div class='juschain-captcha__text'>Seu acesso foi validado "
                  "com sucesso.</span></div>";

EM_JS(void, juschainElement, (const char *content, int solved, char *challenge), {
  if (solved)
    document.dispatchEvent(
        new CustomEvent('juschain', {'detail' : UTF8ToString(challenge)}));
  var juschainElement = document.getElementById('juschain-captcha');
  if (!juschainElement)
    return;
  juschainElement.innerHTML = UTF8ToString(content);
  if (!solved)
    return;
  var input = document.createElement("input");
  input.type = "hidden";
  input.value = UTF8ToString(challenge, 32);
})

void EMSCRIPTEN_KEEPALIVE try_solve() {
  for (int i = 0; i < 15; i++) {
    if (!pow_try(&pow_i))
      continue;
    char challenge[(HASH_LEN * 2) + 1] = {
        0,
    };
    for (int i = 0; i < HASH_LEN; i++)
      sprintf(challenge + (i * 2), "%02hhX", pow_i.seed[i]);
    juschainElement(completed, 1, challenge);
    emscripten_force_exit(0);
    return;
  }
}

int main() {
  if (!emscripten_run_script_int(
          "!document || !document.juschain || typeof "
          "document.juschain.seed !== 'string' || typeof "
          "document.juschain.difficulty !== 'string'"))
    return 1;
  char *seed = emscripten_run_script_string("document.juschain.seed");
  uint8_t difficulty =
      emscripten_run_script_int("document.juschain.difficulty");
  juschainElement(loading, 0, NULL);
  pow_gen(&pow_i, difficulty, seed, strlen(seed));
  emscripten_set_main_loop(try_solve, 0, 0);
  return 0;
}
