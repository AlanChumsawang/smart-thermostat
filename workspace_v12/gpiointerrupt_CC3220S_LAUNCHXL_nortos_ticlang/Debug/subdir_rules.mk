################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"/Applications/ti/ccs1260/ccs/tools/compiler/ti-cgt-armllvm_3.2.1.LTS/bin/tiarmclang" -c -mcpu=cortex-m4 -mfloat-abi=soft -mfpu=none -mlittle-endian -mthumb -Oz -I"/Users/chum/workspace_v12/gpiointerrupt_CC3220S_LAUNCHXL_nortos_ticlang" -I"/Users/chum/workspace_v12/gpiointerrupt_CC3220S_LAUNCHXL_nortos_ticlang/Debug" -I"/Applications/ti/simplelink_cc32xx_sdk_4_30_00_06/source" -I"/Applications/ti/simplelink_cc32xx_sdk_4_30_00_06/kernel/nortos" -I"/Applications/ti/simplelink_cc32xx_sdk_4_30_00_06/kernel/nortos/posix" -DNORTOS_SUPPORT -gdwarf-3 -march=armv7e-m -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)" -I"/Users/chum/workspace_v12/gpiointerrupt_CC3220S_LAUNCHXL_nortos_ticlang/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

build-1582625247: ../gpiointerrupt.syscfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: SysConfig'
	"/Applications/ti/ccs1260/ccs/utils/sysconfig_1.19.0/sysconfig_cli.sh" --script "/Users/chum/workspace_v12/gpiointerrupt_CC3220S_LAUNCHXL_nortos_ticlang/gpiointerrupt.syscfg" -o "syscfg" -s "/Applications/ti/simplelink_cc32xx_sdk_4_30_00_06/.metadata/product.json" --compiler ticlang
	@echo 'Finished building: "$<"'
	@echo ' '

syscfg/ti_drivers_config.c: build-1582625247 ../gpiointerrupt.syscfg
syscfg/ti_drivers_config.h: build-1582625247
syscfg/ti_utils_build_linker.cmd.genlibs: build-1582625247
syscfg/syscfg_c.rov.xs: build-1582625247
syscfg/ti_utils_runtime_model.gv: build-1582625247
syscfg/ti_utils_runtime_Makefile: build-1582625247
syscfg: build-1582625247

syscfg/%.o: ./syscfg/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"/Applications/ti/ccs1260/ccs/tools/compiler/ti-cgt-armllvm_3.2.1.LTS/bin/tiarmclang" -c -mcpu=cortex-m4 -mfloat-abi=soft -mfpu=none -mlittle-endian -mthumb -Oz -I"/Users/chum/workspace_v12/gpiointerrupt_CC3220S_LAUNCHXL_nortos_ticlang" -I"/Users/chum/workspace_v12/gpiointerrupt_CC3220S_LAUNCHXL_nortos_ticlang/Debug" -I"/Applications/ti/simplelink_cc32xx_sdk_4_30_00_06/source" -I"/Applications/ti/simplelink_cc32xx_sdk_4_30_00_06/kernel/nortos" -I"/Applications/ti/simplelink_cc32xx_sdk_4_30_00_06/kernel/nortos/posix" -DNORTOS_SUPPORT -gdwarf-3 -march=armv7e-m -MMD -MP -MF"syscfg/$(basename $(<F)).d_raw" -MT"$(@)" -I"/Users/chum/workspace_v12/gpiointerrupt_CC3220S_LAUNCHXL_nortos_ticlang/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


