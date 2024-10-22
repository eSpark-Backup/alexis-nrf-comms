# alexis-nrfcomms
 # STM32CubeIDE Setup and Project Workflow Guide
 To develop firmware for IoT Button and IoT Speaker products

## 1. Set Up STM32CubeIDE
#### Step 1: Download and Install STM32CubeIDE
1. Download Link: [STM32CubeIDE Download Page](https://www.st.com/en/development-tools/stm32cubeide.html) 
#### Step 2: Set Up the Development Environment
Once STM32CubeIDE is installed, follow the guide below for environment configuration:
* Environment Setup Guide: [STM32CubeIDE Environment Configuration for both Node and Server](https://github.com/mokhwasomssi/stm32_hal_nrf24l01p).(this is already done in projects , you can skip this step)

## 2. Open an Existing Project in STM32CubeIDE
To open an already created STM32 project:

1. Launch STM32CubeIDE.
2. Go to File → Open Projects from File System.
3. Browse and select the project directory that contains the .ioc (CubeMX configuration) and source files.
4. Click Finish to import the project into the workspace.
5. The project will now be visible in the Project Explorer.
   
## 3. Build and Upload Code to STM32
### Step 1: Build the Project
1. Once the project is open, right-click the project folder in the Project Explorer.
2. Select Build Project or press Ctrl+B to compile the code.
If the build succeeds, you will see the output in the Console window showing that the project has been successfully built.
### Step 2: Upload Code to STM32 Microcontroller
You can upload the code to the STM32 board via:
1. Connect your STM32 board to your PC using a USB cable.
2. In STM32CubeIDE, click the Debug button or go to Run → Debug As → STM32 MCU C/C++ Application.
3. Ensure your board is detected. If not, configure the ST-Link settings (for USB) or UART settings accordingly.

## 4. View Output Using CoolTerm
You can use CoolTerm to see the UART output from your Node and Server. CoolTerm is a serial terminal application that helps you monitor serial communication over a selected COM port.
### Step 1: Download and Install CoolTerm
Download and install CoolTerm by following this link:
CoolTerm Download: [CoolTerm Download Page](https://download.cnet.com/coolterm/3000-2094_4-10915882.html)
### Step 2: Connect to the STM32 UART
Once CoolTerm is installed:
1. Launch CoolTerm.
2. In the Options menu:
* Select the correct COM port for your STM32 board.
* Set the Baud Rate (typically 9600 or 115200, as defined in your project).
* Leave other settings as default unless otherwise specified.
3. Click OK to save settings.
### Step 3: View the Output
1. Open the COM port by clicking the Connect button in CoolTerm.
2. You should now see the output from the STM32 board, such as logs from your Node and Server communication.
3. You can disconnect at any time by clicking Disconnect.




