
#include <iostream>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>

using namespace std;

int main(int argc, char* argv)
{
	cout << "c++ embedding mono test\n";

	//mono_set_assemblies_path("");

	MonoDomain* domain = mono_jit_init("script_domain");
	
	MonoAssembly* assembly = mono_domain_assembly_open(domain, "G:\\wind\\tools\\cpp_embed_mono\\script\\Program.dll");
	MonoImage* image = mono_assembly_get_image(assembly);

	MonoClass* mainClass = mono_class_from_name(image, "MonoCsharp", "MainTest");

	MonoMethodDesc* methodDesc = mono_method_desc_new("MonoCsharp.MainTest:Main", true);
	MonoMethod* method = mono_method_desc_search_in_class(methodDesc, mainClass);
	mono_method_desc_free(methodDesc);

	mono_runtime_invoke(method, NULL, NULL, NULL);

	mono_runtime_invoke(method, NULL, NULL, NULL);

	mono_jit_cleanup(domain);

	return 0;
}