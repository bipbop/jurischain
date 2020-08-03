# JurisChain
![browser](./images/browser.png)

Instead of challenging the humans who are working at the terminals, it would be better to challenge the terminals themselves. It's all about balancing out the client's workload so they won't be a victim of a DDoS attack. Inspired by Blockchain technology.

## About

One knows that the computers which serve the publics' courtrooms have suffered from overloads and [DDoS (distributed denial of service) attacks](https://pt.wikipedia.org/wiki/Ataque_de_nega%C3%A7%C3%A3o_de_servi%C3%A7o). Those efforts harm the country's web portal by temporarily shutting it down, bothering the Brazilian judiciary, the lawyers, as well as several different parts of law processes. 

Techniques such as sophisticated firewalls and captchas on the other hand haven't been efficient when it comes to controlling the publics' access, thanks to society's natural process of technological innovation. ([1](https://projurisbrasil.jusbrasil.com.br/artigos/189717091/saiba-o-que-sao-os-captchas-e-como-e-possivel-dribla-los-na-hora-de-fazer-um-acompanhamento-processual-mais-rapido-e-eficiente?ref=feed), [2](https://frradvogados.jusbrasil.com.br/artigos/595582536/big-data-e-acesso-a-informacao-a-legalidade-do-uso-de-bots-robos?ref=feed)) Along with that, several different companies and citizens consume this data without worrying about the stability of the judiciary, be it because of malpractice or disparity.

We remind you, that solutions solutions such as NoCaptcha, from companies abroad, have gotten monetary profit from the citizens', lawyers' and the judiciary's time to solve problems from a computational perspective, monitoring the users with the objects of selling advertisements, and providing the problem and solution simultaneously for access control. The users of the country's judiciary web portal instruct the computers of companies from abroad and to those we let loose total and irrestricted access to public information from our country.  

On behalf of this, we've created a Blockchain based solution that without the expense of public resources nor the time of public workers, infrastructure and safety costs so that the people and the companies can access the courtrooms in a proportional way. The solutions aims to be simple and more efficient than international ones by being completed internalised, open, being in [agreement with the article 4 of the law number 11.419](http://www.planalto.gov.br/ccivil_03/_Ato2004-2006/2006/Lei/L11419.htm#art14)

> Article 14. The systems to be developed by the public bodies of the judiciary should use, preferably, open source programs, accessible in an uninterruptible manner by the means of a worldwide network of computers, priorizing their standardization. 

## About

![keccak](https://keccak.team/assets/img/colors/blue/logo_big.png)

The solutions have been built over the work of the [Keccak team](https://keccak.team/), winner of the [NIST (National Institute of Standards and Technology) competition](https://www.nist.gov/), replacing the SHA-1 and SHA-2 predecessors. This is an international standard and has been chosen precisely thanks to its security and conformity, not benefitting one specific user in detriment of the other. Like this, encouraging the usage of fair consumption of the platforms that implement the solution. 

1. https://csrc.nist.gov/csrc/media/projects/hash-functions/documents/keccak-slides-at-nist.pdf

## Portability- Usage of Public APIs, Apps, Web Portals and Infrastructure.

![C Language](./images/c_128x128.png)

The implementation is made in C (a low-level programming language), guaranteeing that all the platforms profit from the technology thanks to its portability. The challenge can be used by public APIs, apps or web portals, avoiding the overload of services.

> The core C language is extremely portable. The standard Unix implementation is the GNU C compiler, which is ubiquitous not only in open-source Unixes but modern proprietary Unixes as well. GNU C has been ported to Windows and classic MacOS, but is not widely used in either environment because it lacks portable bindings to the native GUI.
> 
> The standard I/O library, mathematics routines, and internationalization support are portable across all C implementations. File I/O, signals, and process control are portable across Unixes provided one takes care to use only the modern APIs described in the Single Unix Specification; older C code often has thickets of preprocessor conditionals for portability, but those handle legacy pre-POSIX interfaces from older proprietary Unixes that are obsolete or close to it in 2003.
> 
> C portability starts to be a more serious problem near IPC, threads, and GUI interfaces. We discussed IPC and threads portability issues in Chapter 7. The real practical problem is GUI toolkits. A number of open-source GUI toolkits are universally portable across modern Unixes and to Windows and classic MacOS as well — Tk, wxWindows, GTK, and Qt are four well-known ones with source code and documentation readily discoverable by Web search. But none of them is shipped with all platforms, and (for reasons more legal than technical) none of these offers the native-GUI look and feel on all platforms. We gave some guidelines for coping in Chapter 15.
> 
> Volumes have been written on the subject of how to write portable C code. This book is not going to be one of them. Instead, we recommend a careful reading of Recommended C Style and Coding Standards [Cannon] and the chapter on portability in The Practice of Programming [Kernighan-Pike99].

[1] http://www.catb.org/~esr/writings/taoup/html/ch17s05.html

### Prepared for the Web

![EMCC](./images/Emscripten_logo_full.png)

We use EMCC (Emscripten Compiler Frontend) to deliver a fast code code to the browsers, so that they'll be highly efficient at the challenges that come with the challenge of bringing an excellent user experience.

## API

### Web Browser
```html
  <link href="./style.css" rel="stylesheet" type="text/css" />
  <script>
    /* Challenge Configuration */
    window.jurischain = {
      seed: 'TrueRandomValue',
      difficulty: 10,
    };
    /* generates an event when solved */
    document.addEventListener('jurischain', ({ detail: response }) => console.log(response));
  </script>
  <!-- Elemento -->
  <div id="jurischain-captcha"></div>
  <script src="./jurischain.js"></script>
```

![on-browser](./images/photo.jpg)

### C
```c
void jurischain_gen(jurischain_ctx_t *challenge, uint8_t difficulty, const void *seed, size_t inlen);

- Creates a new challenge with a certain degree of complexity and a seed.
```
```c
int jurischain_verify(jurischain_ctx_t *challenge)

- Receives a pointer with a challenge and verifies if it can solve the challenge, returning to 1 if it solves it or 0 in case it doesn't.
```
```c
int jurischain_try(jurischain_ctx_t *challenge)

- Receives a pointer with a challenge and tries to solve it, returning 1 in case it does and 0 if it doesn't. 
```
### NodeJS
![Node Logo](https://software.intel.com/sites/default/files/managed/fa/a0/Runtime-logo-Node.jpg)
```sh
$ make
# sudo make install
# sudo ldconfig
$ cd interfaces/node
$ npm install
```


### PHP
![PHP Logo](./images/new-php-logo.png)

#### Instalação

```sh
$ make
# sudo make install
# sudo ldconfig
$ cd interfaces/php
$ phpize
$ ./configure
$ make
$ make test
# sudo make install
```

#### Usage

```php
/**Creates a new challenge*/
$seed = openssl_random_pseudo_bytes(32);
$difficulty = 10;

$jurischain = jurischain_gen($difficulty, $seed);
while (!jurischain_try($jurischain));
var_dump(jurischain_get($jurischain));

$jurischain_new = jurischain_gen($difficulty, $seed);
jurischain_set($jurischain_new, $_POST["jurischain"]);
var_dump(jurischain_verify($jurischain_new));
```

## Statistics

![on-browser](./images/multicomplexity.jpg)

To generate a chart of the number of tries/degree of dificulty of the challenge, do the following: 

```bash
make all
virtualenv .env/
pip install -r requirements.txt
python3 genstats.py
```
The results will be in the folder `stats/`. 
