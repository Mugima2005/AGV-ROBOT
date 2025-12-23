# AGV-ROBOT
Problem Statement:
In many manufacturing industries, manual handling of materials arriving through load lorries is time-consuming and inefficient. Improper handling and placement of materials in warehouses increases labor dependency and operational delays. There is a need for an automated system to transport and place materials accurately in designated warehouse locations with minimal human intervention.


Solution:
The proposed system is an Automated Guided Vehicle (AGV) integrated with a robotic arm to automate material handling operations. The AGV follows a predefined path using IR sensors to transport materials from the loading area to designated warehouse locations. The destination station is entered through a keypad, and the AGV identifies and reaches the correct location using RFID tags. The robotic arm then performs accurate pick and place operations, reducing manual labor and improving efficiency.


Working of the System:
Initially, the required station number is entered using the keypad, and the selected station is displayed on the LCD. Once the system is started, the AGV follows a predefined path using IR sensors for line following. When the AGV reaches the correct station, it detects the corresponding RFID tag and stops automatically. The robotic arm then performs the pick and place operation to position the material at the designated location. An ultrasonic sensor is used to detect obstacles in the path and prevent collisions, ensuring safe and reliable operation.


Components name	quantity
Wheel	2
BO motor	2
Rfid reader 	1
Rfid tag	9
keypad	1
ir sensor - distance	2
LCD Display	1
Ultrasonic sensor	1
Servo	4
Arduino uno	4
Caster wheel	1
M to F Jumper wire	30
M to M Jumper wire	30
F to F Jumper wire	30
L298N motor driver	1
Breadboard 	1
switch	1
NMC 18650 LI-ION Battery-12v	1
	

FLOWCHART:


 

ARCHITECTURE:

 
CIRCUIT DIAGRAM:

 


