#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>

#include "libplatform/libplatform.h"
#include "v8.h"

class Console{};
// Extracts a C string from a V8 Utf8Value.
const char* ToCString(const v8::String::Utf8Value& value);

void Print(const v8::FunctionCallbackInfo<v8::Value>& args);
void Quit(const v8::FunctionCallbackInfo<v8::Value>& args);

void log(const v8::FunctionCallbackInfo<v8::Value>& args);
void error(const v8::FunctionCallbackInfo<v8::Value>& args);
v8::Local<v8::Object> WrapObject(v8::Isolate* isolate, Console *c);


int main(int /*argc*/, char* argv[]) {
  // Initialize V8.
  v8::V8::InitializeICUDefaultLocation(argv[0]);
  v8::V8::InitializeExternalStartupData(argv[0]);
  std::unique_ptr<v8::Platform> platform = v8::platform::NewDefaultPlatform();
  v8::V8::InitializePlatform(platform.get());
  v8::V8::Initialize();
  // Create a new Isolate and make it the current one.
  v8::Isolate::CreateParams create_params;
  create_params.array_buffer_allocator =
      v8::ArrayBuffer::Allocator::NewDefaultAllocator();
  v8::Isolate* isolate = v8::Isolate::New(create_params);
  {
    v8::Isolate::Scope isolate_scope(isolate);
    // Create a stack-allocated handle scope.
    v8::HandleScope handle_scope(isolate);
	
    // Create a new context.
    v8::Local<v8::Context> context = v8::Context::New(isolate);
    // Enter the context for compiling and running the hello world script.
    v8::Context::Scope context_scope(context);
    // Create a string containing the JavaScript source code.
	
	//create js object
    Console* c = new Console();
    v8::Local<v8::Object> con = WrapObject(isolate, c);
	context->Global()->Set(context, 
		v8::String::NewFromUtf8(isolate, "console", v8::NewStringType::kNormal).ToLocalChecked(),
		con).Check();
	context->Global()->Set(context,
      v8::String::NewFromUtf8(isolate, "quit", v8::NewStringType::kNormal).ToLocalChecked(),
      v8::Function::New(context, Quit).ToLocalChecked()).Check();
	
	std::ifstream jsFile("sample_typescript.js", std::ios::binary);
	
	if (!jsFile.is_open())
	{
		return -1;
	}
	
	std::string fileData((std::istreambuf_iterator<char>(jsFile)), std::istreambuf_iterator<char>());
	
    v8::Local<v8::String> source = v8::String::NewFromUtf8(isolate, fileData.data());
    // Compile the source code.
    v8::Local<v8::Script> script =
        v8::Script::Compile(context, source).ToLocalChecked();
    // Run the script to get the result.
    v8::Local<v8::Value> result = script->Run(context).ToLocalChecked();
    // Convert the result to an UTF8 string and print it.
    v8::String::Utf8Value utf8(isolate, result);
    printf("%s\n", *utf8);
  }
  // Dispose the isolate and tear down V8.
  isolate->Dispose();
  v8::V8::Dispose();
  v8::V8::ShutdownPlatform();
  delete create_params.array_buffer_allocator;
  return 0;
}

const char* ToCString(const v8::String::Utf8Value& value)
{
  return *value ? *value : "<string conversion failed>";
}

void Print(const v8::FunctionCallbackInfo<v8::Value>& args) {
	printf("print called %i\n",args.Length()) ;
   bool first = true;
  for (int i = 0; i < args.Length(); i++) {
    v8::HandleScope handle_scope(args.GetIsolate());
    if (first) {
      first = false;
    } else {
      printf(" ");
    }
    v8::String::Utf8Value str(args.GetIsolate(), args[i]);
    const char* cstr = ToCString(str);
    printf("%s", cstr);
  }
  printf("\n");
  fflush(stdout);
}
void Quit(const v8::FunctionCallbackInfo<v8::Value>& /*args*/) {
	std::exit(0);
}

void log(const v8::FunctionCallbackInfo<v8::Value>& args){
	
	v8::String::Utf8Value str(args.GetIsolate(), args[0]);    
	const char* cstr = ToCString(str);     
	printf("%s\n", cstr);
}

void error(const v8::FunctionCallbackInfo<v8::Value>& args){
	v8::String::Utf8Value str(args.GetIsolate(), args[0]);
	const char* cstr = ToCString(str);
	fprintf(stderr,"%s\n", cstr);
}

v8::Local<v8::Object> WrapObject(v8::Isolate* isolate, Console *c)
{
	v8::EscapableHandleScope handle_scope(isolate);

	v8::Local<v8::ObjectTemplate> class_t;
	v8::Local<v8::ObjectTemplate> raw_t = v8::ObjectTemplate::New(isolate);
    raw_t->SetInternalFieldCount(1);
    raw_t->Set(
		v8::String::NewFromUtf8(isolate, "log", v8::NewStringType::kNormal).ToLocalChecked(),
		v8::FunctionTemplate::New(isolate, log));
	raw_t->Set(
		v8::String::NewFromUtf8(isolate, "error", v8::NewStringType::kNormal).ToLocalChecked(),
		v8::FunctionTemplate::New(isolate, error));
	class_t = v8::Local<v8::ObjectTemplate>::New(isolate,raw_t);
    //create instance
    v8::Local<v8::Object> result = class_t->NewInstance(isolate->GetCurrentContext()).ToLocalChecked();
	//create wrapper
    v8::Local<v8::External> ptr = v8::External::New(isolate,c);
	result->SetInternalField(0,ptr);
	return handle_scope.Escape(result);
}