## This file defines guard clauses for system variables exported by build.mk.
## Include this file in the header of Makefiles that should only be called from build.mk or a similar launch file.

ifndef __root_makefile

$(error Couldn't determine the root makefile location. Don't try to run this file directly. Instead, run build.mk)

endif

ifndef __full_src_dir

$(error Couldn't determine the intended source code location. Don't try to run this file directly. Instead, run build.mk)

endif

ifndef __full_build_dir

$(error Couldn't determine the intended build location. Don't try to run this file directly. Instead, run build.mk)

endif
