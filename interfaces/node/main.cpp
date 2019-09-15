#include <jurischain.h>
#include <nan.h>
#include <stdint.h>

class Jurischain : public Nan::ObjectWrap {
public:
  static NAN_MODULE_INIT(Init) {
    v8::Local<v8::FunctionTemplate> ctor =
        Nan::New<v8::FunctionTemplate>(Jurischain::New);
    static Nan::Persistent<v8::Function> constructor;

    ctor->SetClassName(Nan::New("Jurischain").ToLocalChecked());
    ctor->InstanceTemplate()->SetInternalFieldCount(1);

    Nan::SetPrototypeMethod(ctor, "challengeResponse", Define);
    Nan::SetPrototypeMethod(ctor, "readChallenge", Retrieve);

    Nan::SetPrototypeMethod(ctor, "solveStep", Solve);
    Nan::SetPrototypeMethod(ctor, "verify", Verify);

    constructor.Reset(Nan::GetFunction(ctor).ToLocalChecked());
    Nan::Set(target, Nan::New("Jurischain").ToLocalChecked(),
             Nan::GetFunction(ctor).ToLocalChecked());
  }

private:
  jurischain_ctx_t context;

  Jurischain(uint8_t d, const void *seed, size_t inlen) {
    jurischain_gen(&context, d, seed, inlen);
  }

  static NAN_METHOD(New) {
    if (!info.IsConstructCall()) {
      return Nan::ThrowError(
          Nan::New("Jurischain called without new keyword").ToLocalChecked());
    }
    uint8_t difficult =
        info[0]->IsUndefined() ? 0 : Nan::To<double>(info[0]).FromJust();
    Nan::Utf8String seed(info[1]);

    if (seed.length() <= 0) {
      return Nan::ThrowTypeError("arg must be a non-empty string");
    }
    Jurischain *obj =
        new Jurischain(difficult, (void *)seed.operator*(), seed.length());
    obj->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
  }

  static NAN_METHOD(Verify) {
    Jurischain *obj = Nan::ObjectWrap::Unwrap<Jurischain>(info.Holder());
    info.GetReturnValue().Set(
        Nan::New<v8::Boolean>(jurischain_verify(&obj->context)));
  }

  static NAN_METHOD(Solve) {
    Jurischain *obj = Nan::ObjectWrap::Unwrap<Jurischain>(info.Holder());
    info.GetReturnValue().Set(
        Nan::New<v8::Boolean>(jurischain_try(&obj->context)));
  }

  static NAN_METHOD(Define) {
    Jurischain *obj = Nan::ObjectWrap::Unwrap<Jurischain>(info.Holder());
    Nan::Utf8String seed(info[0]);

    if (seed.length() <= 0) {
      return Nan::ThrowTypeError("arg must be a non-empty string");
    }

    if (seed.length() < HASH_LEN * 2) {
      return Nan::ThrowTypeError("arg must have 32 length");
    }

    const char *seedChar = seed.operator*();

    for (size_t i = 0; i < HASH_LEN; i++) {
      if (sscanf(seedChar + (i * 2), "%02hhX", &obj->context.seed[i]) != 1) {
        return Nan::ThrowTypeError("can't write hex");
      }
    }

    info.GetReturnValue().Set(Nan::New<v8::Boolean>(Nan::True));
  }

  static NAN_METHOD(Retrieve) {
    Jurischain *obj = Nan::ObjectWrap::Unwrap<Jurischain>(info.Holder());

    char str[(HASH_LEN * 2) + 1] = {
        0,
    };
    for (int i = 0; i < HASH_LEN; i++) {
      sprintf(str + (i * 2), "%02hhX", obj->context.seed[i]);
    }

    info.GetReturnValue().Set(Nan::New(str).ToLocalChecked());
  }
};

NODE_MODULE(Jurischain, Jurischain::Init);
