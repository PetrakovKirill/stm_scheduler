.PHONY: all clean build flash


name_project = main


#Список всех Исходных файлов Си
C_FILE += main.c
C_FILE += schedul.c
# C_FILE += sys_clock.c

#Список всех ихсходных файлов Си++
Cpp_FILE  = 

#Список всех исходных файлов Asm
ASM_FILE = startup_stm32f10x_md.s
#====================================================
OPTIMIZATION = 0




#Путь к каталогу с компилятором
# CompilerPATH = /home/kririll/Work/Compilers/gcc-arm-none-eabi-5_4-2016q3

#Префикс
# CompilerPrefix = $(CompilerPATH)/bin/arm-none-eabi-
CompilerPrefix = arm-none-eabi-

#Путь к скрипту компановщика
LinkerPATH = ./mcu/stm32_flash.ld
# LinkerPATH = mcu/arm-gcc-link.ld
# LinkerPATH = mcu/stm32f4xx_flash.ld

#Пути к исходным файлам
CPATH =  ./mcu/periph
CPATH += ./program
CPATH += ./mcu/CMSIS
CPATH += ./std_function

#Путь сохранения объектных файлов
OBJPATH = ./out/obj_files/

#Путь сохранения файлов .hex и .bin
MAIN_OUT_PATH = ./out/hexbin/

#Указание путей относительно корневого каталога проекта
#Пути к заголовочным файлам
INCFILE := -I./mcu/CMSIS/Include
INCFILE += -I./mcu/CMSIS
INCFILE += -I./mcu/periph
INCFILE += -I./program
INCFILE += -I./std_function
# INCFILE += -I$(CompilerPATH)/arm-none-eabi/include
# INCFILE += -I$(CompilerPATH)/arm-none-eabi/include/lib


# Путь к консольному программатору st-link utility
# FLASHER = C:\Program Files (x86)\STMicroelectronics\STM32 ST-LINK Utility\ST-LINK Utility\ST-LINK_CLI.exe 
# Параметры программатора
# Для Windows
# FLASHER_PRAM  = -c SWD UR
# FLASHER_PRAM += -P "$(MAIN_OUT_PATH)$(name_project).bin" 0x08000000
# FLASHER_PRAM += -Rst
# Для Линукс
# [--debug] [--reset] [--format <format>] {read|write} /dev/sgX <path> <addr> <size>
FLASHER = st-flash
FLASHER_PRAM  = write $(MAIN_OUT_PATH)/$(name_project).bin 0x08000000




# Настройка отладчика openocd
# DB_SERVER =  D:\GNU_Tools_ARM_Embedded\openocd\bin/openocd.exe
# OPENOCD_PARAM = -c "source [find interface/stlink-v2.cfg]"
# OPENOCD_PARAM += -c "transport select hla_swd"
# OPENOCD_PARAM += -c "source [find target/stm32f1x.cfg]"

# OPENOCD_PARAM_DEBUG = $(OPENOCD_PARAM)
# OPENOCD_PARAM_DEBUG += -c "gdb_port 3333"
# OPENOCD_PARAM_DEBUG += -c "debug_level 2"
# OPENOCD_PARAM_DEBUG += -c "set WORKAREASIZE 0x2000"
# OPENOCD_PARAM_DEBUG += -c "reset_config srst_only"


#Флаги компиляции
CFLAG = $(INCFILE)
CFLAG += -O$(OPTIMIZATION)
CFLAG += -gdwarf-2
CFLAG += -Wall
CFLAG += -c
CFLAG += -fmessage-length=0
CFLAG += -fno-builtin
CFLAG += -ffunction-sections
CFLAG += -fdata-sections
CFLAG += -msoft-float
CFLAG += -mapcs-frame
CFLAG += -D__thumb2__=1
CFLAG += -mno-sched-prolog
CFLAG += -fno-hosted
CFLAG += -mtune=cortex-m3
CFLAG += -mcpu=cortex-m3
CFLAG += -mthumb
CFLAG += -mfix-cortex-m3-ldrd
CFLAG += -fno-strict-aliasing
CFLAG += -ffast-math
CFLAG += -std=c99

CppFLAGS = $(INCFILE)
CppFLAGS += -O$(OPTIMIZATION)
CppFLAGS += -gdwarf-2
CppFLAGS += -Wall
CppFLAGS += -c
CppFLAGS += -fmessage-length=0
CppFLAGS += -fno-builtin
CppFLAGS += -ffunction-sections
CppFLAGS += -fdata-sections
CppFLAGS += -msoft-float
CppFLAGS += -mapcs-frame
CppFLAGS += -D__thumb2__=1
CppFLAGS += -mno-sched-prolog
CppFLAGS += -fno-exceptions
#CppFLAGS += -fno-hosted
CppFLAGS += -mtune=cortex-m3
CppFLAGS += -mcpu=cortex-m3
CppFLAGS += -mthumb
CppFLAGS += -mfix-cortex-m3-ldrd
#CppFLAGS += -fno-strict-aliasing
CppFLAGS += -ffast-math
#CppFLAGS += -std=c99

#Флаги компановщика
FLAGS_LD = -Xlinker -Map=$(MAIN_OUT_PATH)$(name_project).map
FLAGS_LD += -Wl,--gc-sections
FLAGS_LD += -mcpu=cortex-m3
FLAGS_LD += -mthumb
FLAGS_LD += -static 
FLAGS_LD += -T$(LinkerPATH)

#Флаги Ассемблера
FLAGS_ASM  = -D__ASSEMBLY__
FLAGS_ASM += $(CFLAG)
FLAGS_ASM += -g
FLAGS_ASM += -I. -x assembler-with-cpp
#================================================



#Получения списка обектных файлов
OBJ  = $(patsubst %.c,%.o,$(C_FILE))
OBJ += $(patsubst %.cpp,%.o,$(Cpp_FILE))
OBJ += $(patsubst %.s,%.o,$(ASM_FILE))



VPATH  = ./mcu/CMSIS
VPATH += ./mcu/CMSIS/Include
VPATH += ./mcu/periph
VPATH += ./mcu/startup
VPATH += ./mcu/periph
VPATH += ./program
VPATH += ./std_function
VPATH += ./fonts
VPATH += ./out/obj_files
VPATH += ./out/hexbin
VPATH += ./out


build: rm_elf $(name_project).elf

$(name_project).elf: $(OBJ)
	@echo --------------------------
	@echo Linking ...
	@$(CompilerPrefix)gcc $(FLAGS_LD)  $(OBJPATH)*.o -o $(MAIN_OUT_PATH)$(name_project).elf

	@echo '--------------------------------------------------------------------'
	
	@$(CompilerPrefix)objcopy -O binary $(MAIN_OUT_PATH)$(name_project).elf $(MAIN_OUT_PATH)$(name_project).bin
	@$(CompilerPrefix)objcopy -O ihex $(MAIN_OUT_PATH)$(name_project).elf $(MAIN_OUT_PATH)$(name_project).hex
	@$(CompilerPrefix)size $(MAIN_OUT_PATH)$(name_project).elf $(MAIN_OUT_PATH)$(name_project).hex
	@echo Build Comlite.



%.o: %.c
	@echo Compiling $<
	@$(CompilerPrefix)gcc $(CFLAG) $< -o $(OBJPATH)$@


%.o: %.cpp
	@echo Compiling $<
	@$(CompilerPrefix)gcc $(CppFLAGS) $< -o $(OBJPATH)$@


%.o: %.s
	@echo Compiling $<
	@$(CompilerPrefix)gcc $(FLAGS_ASM) $< -o $(OBJPATH)$@

clean:
	@echo Delete object files...
	@rm -rf ./out/obj_files/*.o

rm_elf:
	@rm -f ./out/hexbin/$(name_project).elf


flash:
	$(FLASHER) $(FLASHER_PRAM)

debug:
	# st-util &
	$(CompilerPrefix)gdb $(MAIN_OUT_PATH)$(name_project).elf
# $(DB_SERVER) $(OPENOCD_PARAM_DEBUG)

armGDB:
	@$(CompilerPrefix)gdb /out/hexbin/main.elf
	target remote localhost:3333
