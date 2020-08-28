################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
D:\sloeber\arduinoPlugin\packages\arduino\hardware\avr\1.6.23\libraries\SoftwareSerial\src\SoftwareSerial.cpp 

LINK_OBJ += \
.\libraries\SoftwareSerial\src\SoftwareSerial.cpp.o 

CPP_DEPS += \
.\libraries\SoftwareSerial\src\SoftwareSerial.cpp.d 


# Each subdirectory must supply rules for building sources it contributes
libraries\SoftwareSerial\src\SoftwareSerial.cpp.o: D:\sloeber\arduinoPlugin\packages\arduino\hardware\avr\1.6.23\libraries\SoftwareSerial\src\SoftwareSerial.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"D:\sloeber\arduinoPlugin\packages\arduino\tools\avr-gcc\5.4.0-atmel3.6.1-arduino2/bin/avr-g++" -c -g -Os -Wall -Wextra -std=gnu++11 -fpermissive -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -Wno-error=narrowing -MMD -flto -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=10802 -DARDUINO_AVR_NANO -DARDUINO_ARCH_AVR     -I"D:\sloeber\arduinoPlugin\packages\arduino\hardware\avr\1.6.23\cores\arduino" -I"D:\sloeber\arduinoPlugin\packages\arduino\hardware\avr\1.6.23\variants\eightanaloginputs" -I"D:\sloeber\arduinoPlugin\libraries\CStringBuilder-master\src" -I"D:\sloeber\arduinoPlugin\libraries\SD\1.2.3\src" -I"D:\sloeber\arduinoPlugin\packages\arduino\hardware\avr\1.6.23\libraries\SoftwareSerial\src" -I"D:\sloeber\arduinoPlugin\packages\arduino\hardware\avr\1.6.23\libraries\SPI\src" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -D__IN_ECLIPSE__=1 -x c++ "$<"  -o  "$@"
	@echo 'Finished building: $<'
	@echo ' '


