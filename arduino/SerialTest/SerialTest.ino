//@uart
#define DEBUG
#define BAUDRATE 9600
#define valueNum 10
#define TIMEOUT 500
#define startchar '['
#define endchar ']'
#define splitchar ','
#define endcount 3

char instruction;
int16_t value[valueNum];
int8_t comma = 0;
String inString = "";

void serialEvent()
{
    while (Serial.available())
    {
        char inChar = Serial.read();
        switch (inchar)
        {
        case startchar:
            inString = "";
            comma = 0;
            break;
        case endchar:
            RXcomplete = true;
        case splitchar:
            if (comma)
            {
                value[comma] = inString.toInt();
            }
            else
            {
                instruction = inchar;
            }
            if (comma++ > valueNum)comma = valueNum;
            break;
        default:
            inString += inchar;
        }
    }
}

void setup()
{
    Serial.begin(BAUDRATE);
}

void loop()
{
    if (RXcomplete)
    {
        Serial.print("CMD: ");
        Serial.println(instruction);
        for (int i = 0; i < valueNum; i++)
        {
            Serial.print(valur[i]);
            Serial.print('\t');
        }
        Serial.print('\n');
        reciveComplete();
    }
}