#include <stdio.h>
#include <stdint.h>

#include "emscripten.h"
#include "sha3pow.h"
#include "string.h"

pow_ctx_t pow_i;
uint8_t trys = 0;
size_t len = 32;

char *loading = "<div class='robot-captcha__marker'><div class='robot-captcha__checkbox--loading'></div></div>"
    "<div class='robot-captcha__text'>Aguarde enquanto validamos seu acesso.</span></div>";

char *completed = "<div class='robot-captcha__marker'><div class='robot-captcha__checkbox--completed'></div></div>"
    "<div class='robot-captcha__text'>Seu acesso foi validado com sucesso.</span></div>";

EM_JS(void, robotElement, (const char *content, int solved, char *challenge), {
  if (solved) document.dispatchEvent(new CustomEvent('robotCaptcha', {
    'detail': UTF8ToString(challenge)
  }));
  var robotCaptchaElement = document.getElementById('robot-captcha');
  if (!robotCaptchaElement) return;
  robotCaptchaElement.innerHTML = UTF8ToString(content);
  if (!solved) return;
  var input = document.createElement("input");
  input.type = "hidden";
  input.value = UTF8ToString(challenge, 32);
})

EM_JS(void, debug, (uint8_t difficulty, char *seed), {
  console.log('Inicializando RobotCaptcha', {
    "difficulty": difficulty,
    "seed": UTF8ToString(seed),
  });
})

void EMSCRIPTEN_KEEPALIVE try_solve() {
    for (int i = 0; i < 15; i++) {
      if (!pow_try(&pow_i)) continue;
      char challenge[(HASH_LEN*2)+1] = {0,};
      for (int i=0; i < HASH_LEN; i++) sprintf(challenge + (i * 2), "%02X", pow_i.seed[i]);
      robotElement(completed, 1, challenge);
      emscripten_force_exit(0);
      return;
    }
}

int main() {
    if (!emscripten_run_script_int("!document || !document.robotCaptcha || typeof document.robotCaptcha.seed !== 'string' || typeof document.robotCaptcha.difficulty !== 'string'")) return 1;
    char *seed = emscripten_run_script_string("document.robotCaptcha.seed");
    uint8_t difficulty = emscripten_run_script_int("document.robotCaptcha.difficulty");
    debug(difficulty, seed);
    robotElement(loading, 0, NULL);
    pow_gen(&pow_i, difficulty, seed, strlen(seed));
    emscripten_set_main_loop(try_solve, 0, 0);
    return 0;
}
