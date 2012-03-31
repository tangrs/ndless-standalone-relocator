# Ndless standalone binary relocator

This is a tool for (at the moment) Ndless developers who want automatic relocation of their global data structures.

It simply tacks on a startup file and a list of relocations onto your existing .tns files. This startup file will read the list of relocations and relocate it at runtime automatically without an extra ELF loader.

Unfortunately, this process requires you to run an extra program during the compiling process. This program modifies your binary to auto relocate itself.

This isn't calculator model dependant and will work on any version supported by Ndless.

## Differences with Ndless-Elfloader (https://github.com/tangrs/ndless-elfloader)

### Advantages

* Ndless-Elfloader requires a hook to be installed on the program loader which can be tricky to maintain as new OS's are released. Ndless-Elfloader is also very large in size (~50KB). This loader is entirely self sustained and works across different calculator versions.

* By being entirely sustained, you also don't have to require your users to install anything else.

* Ndless-Elfloader binaries are larger since .elf files hold a lot of redundant information like unused and debugging symbols and generally have a lot of overhead. This standalone relocator only contains the nessecary information required to relocate.

* This standalone binary relocator gives most of the work to the host instead of the calculator. This reduces the amount of work the calculator has to do and results in faster loading than Ndless-Elfloader (which delegates all the work to the calculator).

### Disadvantages

* Ndless-Elfloader requires no custom made programs. This means that end developers don't have to spend time working out how to use a new tool and can use existing tools on his machine.

* This standalone loader has lots of bits and pieces which can be annoying to maintain.


## Building

First, you need to make the startup binary. Run ```make``` in the ```arm``` directory. This will create the ```startup.bin``` file you will need later.

Next, you need to build the tool that will modify your binary. Run ```make``` in the ```host``` directoy.

If you're using Mac OS X, you may need to install MacPorts and install libelf.

If you're using Linux, you will need to install libelf. Also, if the compiler complains about how it can't find /opt/local/include, simply delete the relevant bits in the GCCFLAGS and LDFLAGS in the Makefile.

If you're using Windows, you might need to readjust the Makefile a little to produce a .exe instead. You will also probably need libelf. Unfortunately, I'm not experienced with compiling on Windows. You may need to install it through whatever package manager you use (most likely in a Cygwin environment).

## Using in your own projects

You need to add ```-Wl,--emit-relocs``` to your LDFLAGS in your project Makefile.

When you finish compiling your .tns binary, feed them both to the ```makeself``` program along with your ```startup.bin``` file.

## Makeself

Syntax: ```makeself inputelf inputbin output startupfile```

```inputelf``` is your .elf file

```inputbin``` is your .tns file

```output``` is your output .tns file

```startupfile``` is the ```startup.bin``` file you obtained earlier

The resulting .tns file will have automatic runtime relocation.

## Licensing

See each folder for their individual licenses.