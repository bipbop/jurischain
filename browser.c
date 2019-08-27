#include <stdio.h>
#include <stdint.h>

#include "emscripten.h"
#include "sha3pow.h"

uint8_t answer[32];
uint8_t challenge[33];
uint8_t rand[32];
uint8_t trys = 0;
size_t len = 32;

char *loading = "<div class='robot-captcha__marker'><div class='robot-captcha__checkbox--loading'></div></div>"
    "<div class='robot-captcha__text'>Aguarde enquanto validamos seu acesso.</span></div>";

char *completed = "<div class='robot-captcha__marker'><div class='robot-captcha__checkbox--completed'></div></div>"
    "<div class='robot-captcha__text'>Seu acesso foi validado com sucesso.</span></div>";

EM_JS(void, robotElement, (const char *content, int solved, char *challenge), {
  if (solved) document.dispatchEvent(new CustomEvent('robotCaptcha', {
    'detail': challenge
  }));
  var robotCaptchaElement = document.getElementById('robot-captcha');
  if (!robotCaptchaElement) return;
  robotCaptchaElement.innerHTML = UTF8ToString(content);
  if (!solved) return;
  var input = document.createElement("input");
  input.type = "hidden";
  input.value = UTF8ToString(challenge, 32);
});


EM_JS(void, debug, (uint8_t difficulty, char *seed), {
  console.log('Inicializando RobotCaptcha', {
    "difficulty": difficulty,
    "seed": UTF8ToString(seed),
  });
});

void EMSCRIPTEN_KEEPALIVE try_solve() {
    for (int i = 0; i < 15; i++) {
      if (!pow_try(challenge, answer, rand)) continue;
      char challenge[65] = {0,};
      for (int i=0; i < len; i++) sprintf(challenge + (i * 2), "%02X", answer[i]);
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
    memcpy(rand, seed, sizeof(rand));
    pow_gen(difficulty, challenge, rand);
    emscripten_set_main_loop(try_solve, 0, 0);
    return 0;
}
