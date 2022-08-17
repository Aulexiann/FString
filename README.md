# FString
C++ Fixed String. 

Header-only. Semi-modern style. Nultermed. Sized. Constexpr. Noexcept.

ImGui (InputText, PlainText) and std::ostream (overload) ready.

With default preset: char[14], nulterm-char, size-uint8_t; alignof 1 byte.

## Usage and targets
With any DOD paradigm solutions on small component-structs you can't just hold title-string or smth like it. Raw char array with required C functions, std::string with 32b size + 8b align + non-guaranteed 16b storage... Even file io can't be trivial like Write(data, size), and easy to take performance penalty like format and put strings every frame to ImGui.

A lot of another "fixed string" solutions are not enough to me. Always max size restriction, no size (need manual calc length every call), no string operations or no convertions, etc. 

So now you can use FString instead any others. Hold as field, concat prefix or postfix, mutate current or make new, with value-convertion or char literal. 

## TODO
-examples block and simplify transfers from current project

-templated version, AddNew()'s and int-float selector

-more informative errors and more asserts

-tests to check ranges and convertions

-generalize nultermation (data[size]='\0')

-all operations as "atomic" (not like floatings here)
