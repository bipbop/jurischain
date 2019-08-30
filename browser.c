#include <stdint.h>
#include <stdio.h>

#include "emscripten.h"
#include "jurischain.h"
#include "string.h"

jurischain_ctx_t pow_i;
uint8_t trys = 0;
size_t len = 32;

char *loading = "<div class='jurischain-captcha__marker'><div "
                "class='jurischain-captcha__checkbox--loading'></div></div>"
                "<div class='jurischain-captcha__text'>Aguarde enquanto validamos "
                "seu acesso.</span></div>";

char *completed = "<div class='jurischain-captcha__marker'><div "
                  "class='jurischain-captcha__checkbox--completed'></div></div>"
                  "<div class='jurischain-captcha__text'>Seu acesso foi validado "
                  "com sucesso.</span></div>";

EM_JS(void, jurischainElement, (const char *content, int solved, char *challenge), {
  if (solved) {
    var event;
    var solution = UTF8ToString(challenge, 32);
    if (typeof CustomEvent === 'function') {
      event = new CustomEvent('jurischain', {'detail' : solution });
    } else {
      event = document.createEvent('CustomEvent');
      event.initCustomEvent('jurischain', true, false, solution);
    }
    document.dispatchEvent(event);
  }

  var jurischainElement = document.getElementById('jurischain-captcha');
  if (jurischainElement) {
    /* adiciona o conteúdo */
    jurischainElement.innerHTML = UTF8ToString(content);
    if (solved) {
      /* existe o elemento e foi resolvido, logo haverá um input */
      var input = document.createElement("input");
      input.type = "hidden";
      input.name = "jurischain";
      input.value = solution;
      jurischainElement.appendChild(input);
    }
  }

})

void EMSCRIPTEN_KEEPALIVE try_solve() {
  for (int i = 0; i < 15; i++) {
    if (!jurischain_try(&pow_i))
      continue;
    char challenge[(HASH_LEN * 2) + 1] = {
        0,
    };
    for (int i = 0; i < HASH_LEN; i++)
      sprintf(challenge + (i * 2), "%02hhX", pow_i.seed[i]);
    jurischainElement(completed, 1, challenge);
    emscripten_force_exit(0);
    return;
  }
}

int main() {
  if (!emscripten_run_script_int(
          "!document || !document.jurischain || typeof "
          "document.jurischain.seed !== 'string' || typeof "
          "document.jurischain.difficulty !== 'string'"))
    return 1;
  char *seed = emscripten_run_script_string("document.jurischain.seed");
  uint8_t difficulty =
      emscripten_run_script_int("document.jurischain.difficulty");
  jurischainElement(loading, 0, NULL);
  jurischain_gen(&pow_i, difficulty, seed, strlen(seed));
  emscripten_set_main_loop(try_solve, 0, 0);
  return 0;
}
