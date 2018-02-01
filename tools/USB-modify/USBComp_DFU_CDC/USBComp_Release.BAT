SET PATH=E:\keil\ARM\ARMCC\Bin;C:\Perl64\site\bin;C:\Perl64\bin;C:\Program Files (x86)\Python36-32\Scripts\;C:\Program Files (x86)\Python36-32\;E:\Java\jre1.8.0_112\bin;E:\Java\jdk1.8.0_112\bin;c:\Program Files (x86)\Intel\iCLS Client\;c:\Program Files\Intel\iCLS Client\;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\;C:\Program Files\Intel\Intel(R) Management Engine Components\DAL;C:\Program Files\Intel\Intel(R) Management Engine Components\IPT;C:\Program Files (x86)\Intel\Intel(R) Management Engine Components\DAL;C:\Program Files (x86)\Intel\Intel(R) Management Engine Components\IPT;C:\Program Files (x86)\NVIDIA Corporation\PhysX\Common;C:\strawberry\c\bin;C:\strawberry\perl\bin;C:\Program Files\TortoiseSVN\bin:E:\Qt5.6.0\qt\5.6\mingw49_32\bin
SET CPU_TYPE=LPC11U35FHI33/501
SET CPU_VENDOR=NXP
SET UV2_TARGET=USBComp_Release
SET CPU_CLOCK=0x00B71B00
"E:\keil\ARM\ARMCC\Bin\ArmCC" --Via ".\obj\core_cm0.__i"
"E:\keil\ARM\ARMCC\Bin\ArmCC" --Via ".\obj\system_lpc11uxx.__i"
"E:\keil\ARM\ARMCC\Bin\ArmAsm" --Via ".\obj\startup_lpc11uxx._ia"
"E:\keil\ARM\ARMCC\Bin\ArmCC" --Via ".\obj\usbcore.__i"
"E:\keil\ARM\ARMCC\Bin\ArmCC" --Via ".\obj\usbhw.__i"
"E:\keil\ARM\ARMCC\Bin\ArmCC" --Via ".\obj\dfuuser.__i"
"E:\keil\ARM\ARMCC\Bin\ArmCC" --Via ".\obj\cdcuser.__i"
"E:\keil\ARM\ARMCC\Bin\ArmCC" --Via ".\obj\serial.__i"
"E:\keil\ARM\ARMCC\Bin\ArmCC" --Via ".\obj\sbl_iap.__i"
"E:\keil\ARM\ARMCC\Bin\ArmCC" --Via ".\obj\demo.__i"
"E:\keil\ARM\ARMCC\Bin\ArmCC" --Via ".\obj\usbuser.__i"
"E:\keil\ARM\ARMCC\Bin\ArmCC" --Via ".\obj\usbdesc.__i"
"E:\keil\ARM\ARMCC\Bin\ArmCC" --Via ".\obj\ads1014.__i"
"E:\keil\ARM\ARMCC\Bin\ArmCC" --Via ".\obj\ads1100.__i"
"E:\keil\ARM\ARMCC\Bin\ArmCC" --Via ".\obj\pca9555.__i"
"E:\keil\ARM\ARMCC\Bin\ArmCC" --Via ".\obj\lpc11xx_debug.__i"
"E:\keil\ARM\ARMCC\Bin\ArmCC" --Via ".\obj\lpc11xx_uart.__i"
"E:\keil\ARM\ARMCC\Bin\ArmLink" --Via ".\Obj\USBComp.lnp"
"E:\keil\ARM\ARMCC\Bin\fromelf.exe" ".\Obj\USBComp.axf" --i32combined --output ".\Obj\USBComp.hex"
E:\keil\\ARM\BIN\ELFDWT.EXE .\Obj\USBComp.axf BASEADDRESS(0x0)
