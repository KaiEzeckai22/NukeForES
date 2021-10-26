//#include <string>
#define WT 1500
char buffer[50];
char extract[47];
int extractSize = 0;
char nonExponent[47];
int nonExponentSize = 0;
char exponent[47];
int exponentSize = 0;
char ignored[47];
int ignoredSize = 0;
int intSize = 0;
int decimalPlaces = 0;
int tolerance = 0;
int digitsSize = 0;
//String toTest[] = {"0aaKKK-7.23-24", "0aaKKK-7.23.-24", "0aaKKK-7..7-8"};
String toTest[] = {"0aaKKK-7.23eea", "0aaKKK-ee123", "0aaKKK7.34e1a", "0aaKKK7e1e-1", "0aaKKK7.6e.5", "0aaKKK7.2..6e5", "0aaKKK7.66e-3", "0aaKKK7.6e8", "0aaKKK-7.6"};
int toTestSize = sizeof(toTest) / sizeof(toTest[0]);
int start = 0;
int end = 0;
String output = "";
String stringBuffer = "";

#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup()
{
    lcd.begin(16, 2);
    Serial.begin(9600);
    lcd.print("0123456789ABCDEF");
    //test();
}

void loop()
{
    if (Serial.available() > 0)
    {
        String input = Serial.readString();
        //String input = "KKK";
        //input.concat(Serial.readString());
        injectToBuffer(input);
        displayData();
        clearAllArray();
        delay(WT);
    }
    /*
    for (int i = 0; i < toTestSize; i++)
    {
        Serial.println("------------------------------------------");
        injectToBuffer(toTest[i]);
        displayData();
        clearAllArray();
        delay(WT);
    }

    String temp = "KKK";
    for (int i = 0; i < 15; i++)
    {
        temp.concat("1");
        Serial.println("------------------------------------------");
        injectToBuffer(temp);
        displayData();
        clearAllArray();
        delay(WT);
    }*/
}

void injectToBuffer(String thisString)
{
    // INJECT THE STRING TO THE CHARARRAY[50]
    thisString.toCharArray(buffer, 50);
    int size = sizeof(buffer) / sizeof(char);
}

void displayData()
{
    float convertedValue = 0;
    if (detectString())
    {
        int extractValue = extractionProcess();
        convertedValue = convertExtended();

        // SERIAL DISPLAY
        printAll(extractValue, convertedValue);
        // LCD DISPLAY
        displayAll(extractValue, convertedValue);
    }
    else
    {
        Serial.println("NONE FOUND");

        delay(1000);
        allClear();
        row1Display("NONE FOUND");
        delay(3000);
    }
}

void printAll(int validity, float convertedValue)
{
    Serial.println();

    Serial.print("STRING VALUE: ");
    Serial.println(extract);
    Serial.print("STRING IS ");
    switch (validity)
    {
    case 0:
        Serial.println("INVALID");
        break;
    case 1:
        Serial.println("VALID");
        break;
    case 2:
        Serial.println("PARTIALLY VALID");
        break;
    default:
        break;
    }
    if (1 /*validity*/)
    {
        Serial.print("NON EXP: ");
        Serial.println(nonExponent);
        Serial.print("EXP: ");
        Serial.println(exponent);
        Serial.print("IGNORED: ");
        Serial.println(ignored);
        Serial.print("CONVERTED VALUE: ");
        if (digitsSize > 20)
        {
            Serial.println("OVERFLOW");
        }
        else
        {
            Serial.println(convertedValue, tolerance);
        }
        Serial.println();
    }
}

void displayAll(int validity, float convertedValue)
{
    delay(WT);
    allClear();
    row1Display("STRING VALUE:");
    row2Display(extract);
    delay(WT);
    allClear();
    row1Display("STRING IS");
    switch (validity)
    {
    case 0:
        row2Display("INVALID");
        break;
    case 1:
        row2Display("VALID");
        break;
    case 2:
        row2Display("PARTIALLY VALID");
        break;
    default:
        break;
    }
    if (validity)
    {
        delay(WT);
        allClear();
        row1Display("NON EXP: ");
        row2Display(nonExponent);
        delay(WT);
        allClear();
        row1Display("EXP: ");
        row2Display(exponent);
        delay(WT);
        allClear();
        row1Display("IGNORED: ");
        row2Display(ignored);
        delay(WT);
        allClear();
        row1Display("CONVERTED VALUE:");
        if (digitsSize > 20)
        {
            row2Display("OVERFLOW");
        }
        else
        {
            row2Display(convertedValue);
        }
        delay(WT);
        allClear();
    }
}

void displayInvalid()
{
    delay(1000);
    allClear();
    row1Display("STRING VALUE:");
    row2Display(extract);
    delay(3000);
    allClear();
    row1Display("STRING IS");
    row2Display("INVALID");
}

bool detectString()
{
    // GO FIND THE STX / KKK
    start = findIndexSTX();
    if (start == -1)
    {
        return 0;
    }
    for (int i = 0; i < 50; i++)
    {
        // EOL DETECTION
        if (buffer[i] == '\0')
        {
            // MARK THE END OF FILE
            end = i;
            break;
        }
        extract[i] = buffer[i + start];
    }
    // ASSIGN EXTRACT SIZE TO REDUNDANT READING
    // OF EMPTY SPACES IN FOR LOOPS
    extractSize = end - start;
    return 1;
}

int findIndexSTX()
{
    //FIND KKK AND INDEX OTHERWISE RETURN DEFUNC
    for (int i = 0; i <= 50; i++)
    {
        if ((buffer[i] == 'K') && (buffer[i + 1] == 'K') && (buffer[i + 2] == 'K'))
        {
            return (i + 3);
        }
    }
    return -1;
}
//---------------------------------------------------------------------
// CONSTRUCTION SITE
//---------------------------------------------------------------------
/*
double convertToDouble(String this){
	// THIS IS THE NEXT PROBLEM	
}
*/
float convertToFloat(const char *s)
{
    float value = 0, fact = 1;
    int moves = 0;
    //negative
    if (*s == '-')
    {
        s++;
        fact = -1;
    };
    for (int point_seen = 0; *s; s++)
    {
        if (*s == '.')
        {
            point_seen = 1;
            continue;
        };
        double d = *s - '0';
        if (d >= 0 && d <= 9)
        {
            if (point_seen)
                fact /= 10.0d;
            value = value * 10.0d + (float)d;
        };
    };
    return value * fact;
};

//---------------------------------------------------------------------
boolean detectChar(char toComp)
{
    for (int i = 0; i < extractSize; i++)
    {
        if (extract[i] == toComp)
        {
            return true;
        }
    }
    return false;
}

boolean detectCharExp(char toComp)
{
    for (int i = 0; i < exponentSize; i++)
    {
        if (exponent[i] == toComp)
        {
            return true;
        }
    }
    return false;
}

bool checkValid(char toCheck)
{
    // RETURN 0 INVALID, 1 VALID
    if (isDigit(toCheck) || toCheck == '-' || toCheck == '.')
    {
        return 1;
    }
    return 0;
}

bool checkValidExp(char toCheck)
{
    // RETURN 0 INVALID, 1 VALID
    if (isDigit(toCheck) || toCheck == '-')
    {
        return 1;
    }
    return 0;
}

// JUST RESET PREVIOUS ARRAYS
void clearAllArray()
{
    for (int i = 0; i < extractSize; i++)
    {
        extract[i] = '\0';
    }
    for (int i = 0; i < nonExponentSize; i++)
    {
        nonExponent[i] = '\0';
    }
    for (int i = 0; i < exponentSize; i++)
    {
        exponent[i] = '\0';
    }
    for (int i = 0; i < ignoredSize; i++)
    {
        ignored[i] = '\0';
    }
    extractSize = 0;
    nonExponentSize = 0;
    exponentSize = 0;
    ignoredSize = 0;
    digitsSize = 0;
}

// METHODS FOR DISPLAY ------------------------
void row1Display(String toDisplayText, int value)
{
    output = "";
    lcd.setCursor(0, 0);
    output.concat(toDisplayText);
    output.concat(value);
    lcd.print(output);
}

void row1Display(String toDisplayText, String toDisplayText2)
{
    output = "";
    lcd.setCursor(0, 0);
    output.concat(toDisplayText);
    output.concat(toDisplayText2);
    lcd.print(output);
}

void row1Display(String toDisplayText)
{
    output = "";
    lcd.setCursor(0, 0);
    output.concat(toDisplayText);
    lcd.print(output);
}

void row2Display(String toDisplayText, int value)
{
    output = "";
    lcd.setCursor(0, 1);
    output.concat(toDisplayText);
    output.concat(value);
    lcd.print(output);
}

void row2Display(String toDisplayText)
{
    output = "";
    lcd.setCursor(0, 1);
    output.concat(toDisplayText);
    lcd.print(output);
}

void row2Display(float toDisplayFloat)
{
    output = "";
    lcd.setCursor(0, 1);
    lcd.print(toDisplayFloat, tolerance);
}

void row1Clear()
{
    lcd.setCursor(0, 0);
    lcd.print("                ");
}

void row2Clear()
{
    lcd.setCursor(0, 1);
    lcd.print("                ");
}

void allClear()
{
    row1Clear();
    row2Clear();
}

float convertExtended()
{
    int expo = 0;
    expo = atoi(exponent);
    float nonExp = convertToFloat(nonExponent);
    tolerance = 0;

    if ((intSize + decimalPlaces) < 6)
    {
        tolerance = decimalPlaces - expo;
    }
    else
    {
        tolerance = (6 - intSize) - expo;
    }
    // CLIP NEGATIVE TOLERANCES
    if (tolerance < 0)
    {
        tolerance = 0;
    }
    // IDENTIFY IF THE NUMBER EXPANDS (DECIMAL PLACES) AND BY HOW MUCH
    int expansion = 0;
    int expansionType = 0;

    digitsSize = intSize + decimalPlaces;
    if ((expo) >= intSize)
    {
        // EXPAND LEFT
        expansionType = 1;
        expansion = expo - intSize;
        digitsSize = intSize + decimalPlaces + expansion;
    }
    else if ((0 - expo) >= decimalPlaces)
    {
        // EXPAND RIGHT
        expansionType = 2;
        expansion = (expo)-decimalPlaces;
        digitsSize = intSize + abs((expo)-decimalPlaces);
    }

    Serial.print("EXPO: ");
    Serial.println(expo);

    Serial.print("INT SIZE: ");
    Serial.println(intSize);

    Serial.print("DEC SIZE: ");
    Serial.println(decimalPlaces);

    Serial.print("EXPANSION TYPE: ");
    Serial.println(expansionType);

    Serial.print("EXPANSION SIZE: ");
    Serial.println(expansion);

    Serial.print("TOLERANCE SIZE: ");
    Serial.println(tolerance);

    Serial.print("DIGITS SIZE: ");
    Serial.println(digitsSize);

    float converted = 0;
    if (expo == NULL)
    {
        // NON EXP FILTER
        tolerance = decimalPlaces;
        converted = nonExp; //inaccurate sometimes (more than 1 or 2 decimal places)
    }
    else if ((expo + decimalPlaces) > 6)
    {
        double nuke = 1;
      	for(int i = 0; i < expo; i++){
        	nuke = nuke * 10;
        }
      	converted = nonExp * nuke;
      	// OVF FILTER
        //converted = nonExp * pow(10, expo);
        return converted - 0.001 ; //this is the unit limit I think, *10 or move to right will result to ovf
    }
    else
    {
        // DEFAULT FILTER
        converted = nonExp * pow(10, expo);
    }
    return converted;
}

int extractionProcess()
{
    intSize = 0;
    decimalPlaces = 0;
    nonExponentSize = 0;
    exponentSize = 0;
    digitsSize = 0;
    int state = 0;
    bool dotFound = 0;
    bool nonExpFound = 0;
    bool expFound = 0;
    bool eFound = 0;
    bool partiality = 0;
    bool negFound = 0;
    bool negAftE = 0;

    bool invalid = 0;
    int backTrack = 0;

    // DETECT INVALID START (err,a.b,a23)
    if (!checkValid(extract[0]))
    {
        //state = 4;
        return 0;
    }

    for (int i = 0; i < extractSize; i++)
    {
        // IDENTIFUCKATION
        switch (extract[i])
        {
        case '-':
            switch (state)
            {
            case 0:
                if (i == 0)
                {
                    negFound = 1;
                }
                else
                {
                    if (intSize)
                    {
                        partiality = 1;
                        state = 3;
                    }
                    else
                    {
                        //state = 4;
                        return 0;
                    }
                }
                break;
            case 1:
                partiality = 1;
                state = 3;
                break;
            case 2:
                if (!negAftE)
                {
                    negAftE = 1;
                }
                else
                {
                    partiality = 1;
                    state = 3;
                }
                break;
            default:
                break;
            }

            break;
        case '.':
            switch (state)
            {
            case 0:
                if (intSize)
                {
                    state = 1;
                }
                else
                {
                    //state = 4;
                    return 0;
                }
                break;
            case 1:
                if (!decimalPlaces)
                {
                    backTrack = 1;
                }
                partiality = 1;
                state = 3;
                //return 0;
                break;
            case 2:
                partiality = 1;
                state = 3;
                backTrack = 1;
                break;
            default:
                break;
            }
            break;
        case 'e':
            switch (state)
            {
            case 0:
                if (intSize)
                {
                    if (exponentSize)
                    {
                        partiality = 1;
                        state = 3;
                        backTrack = 1;
                    }
                    else
                    {
                        state = 2;
                    }
                }
                else
                {
                    //state = 4;
                    return 0;
                }
                break;
            case 1:
                if (decimalPlaces)
                {
                    expFound = 1;
                    state = 2;
                }
                else
                {
                    partiality = 1;
                    state = 3;
                    backTrack = 1;
                }
                break;
            case 2:
                partiality = 1;
                state = 3;
                backTrack = 1;
                break;
            default:
                break;
            }
            break;
        }

        if (!checkValid(extract[i]) && extract[i] != 'e')
        {
            partiality = 1;
            state = 3;
            /*
            if (extract[i - 1] == 'e')
            {
                backTrack = 1;
            }*/
        }

        // EXTRACTION
        switch (state)
        {
        case 0: // NON EXPONENT INTEGER
            if (isDigit(extract[i]))
            {
                intSize++;
            }
            nonExponent[nonExponentSize] = extract[i];
            nonExponentSize++;
            break;
        case 1: // NON EXPONENT DECIMAL
            if (!(extract[i] == '.'))
            {
                decimalPlaces++;
            }
            nonExponent[nonExponentSize] = extract[i]; // NEEDS SHIFT i @ nonExponent
            nonExponentSize++;
            break;
        case 2:
            if (!(extract[i] == 'e'))
            {
                exponent[exponentSize] = extract[i]; // NEEDS SHIFT i @ exponent
                exponentSize++;
            }
            else if (i == (extractSize - 1))
            {
                ignored[0] = 'e';
                partiality = 1;
            }
            break;
        case 3: // EXPONENT AND IGNORED
            //ignored[i - (negFound + intSize + dotFound + decimalPlaces + exponentSize) - 1] = extract[i]; // NEEDS SHIFT i @ ignored
            if (backTrack)
            {
                // ADD IT BACK IN
                if (extract[i - 1] == 'e')
                {
                    ignored[ignoredSize] = 'e';
                }
                else
                {
                    ignored[ignoredSize] = '.';
                    nonExponent[i - 1] = '\0';
                }
                ignoredSize++;
                backTrack = 0;
            }
            ignored[ignoredSize] = extract[i];
            ignoredSize++;
            break;
        case 4:
            ignored[ignoredSize] = extract[i];
            ignoredSize++;
            break;
        default:
            break;
        }

        Serial.println();
        Serial.print("EXT: ");
        Serial.print(extract[i]);
        Serial.print(" / STATE: ");
        Serial.print(state);
        Serial.print(" / Int: ");
        Serial.print(intSize);
        Serial.print(" / Dec: ");
        Serial.print(decimalPlaces);
        Serial.print(" / Expo: ");
        Serial.print(exponentSize);
        Serial.print(" / Ign: ");
        Serial.print(ignoredSize);
        Serial.println();
    }
    Serial.println();
    Serial.print("Neg: ");
    Serial.print(negFound);
    Serial.print(" / Int: ");
    Serial.print(intSize);
    Serial.print(" / Dot: ");
    Serial.print(dotFound);
    Serial.print(" / Dec: ");
    Serial.print(decimalPlaces);
    Serial.print(" / Expo: ");
    Serial.print(exponentSize);
    Serial.print(" / Ign: ");
    Serial.print(ignoredSize);
    Serial.println();
  
    if (state == 4)
    {
        return 0;
    }
    if (!partiality)
    {
        return 1;
    }

    return 2;
}