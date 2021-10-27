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
int start = 0;
int end = 0;
String output = "";
int type = 0;

#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup()
{
    lcd.begin(16, 2);
    Serial.begin(9600);
}

void loop()
{
    if (Serial.available() > 0)
    {
        String input = Serial.readString();
        injectToBuffer(input);
        displayData();
        clearAllArray();
        delay(WT);
        allClear();
    }
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
        allClear();
        row1Display("NONE FOUND");
        delay(WT);
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
    if (validity)
    {
        Serial.print("NON EXP: ");
        Serial.println(nonExponent);
        Serial.print("EXP: ");
        Serial.println(exponent);
        Serial.print("IGNORED: ");
        Serial.println(ignored);
        Serial.print("CONVERTED VALUE: ");
        Serial.println(convertedValue, tolerance);
        if ((type == 0 || type == 1) && ((abs(convertedValue) > 4294967040.0) || ((intSize + decimalPlaces) > 7)))
        {
            Serial.println("NUMBER TOO LARGE");
        }
        else if (type == 2 && (digitsSize + 2) > 16)
        {
            Serial.println("OUT OF BOUNDS");
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
        if (exponentSize)
        {
            delay(WT);
            allClear();
            row1Display("EXP: ");
            row2Display(exponent);
        }
        if (ignoredSize)
        {
            delay(WT);
            allClear();
            row1Display("IGNORED: ");
            row2Display(ignored);
        }
        delay(WT);
        allClear();
        row1Display("CONVERTED VALUE:");
        if ((type == 0 || type == 1) && ((abs(convertedValue) > 4294967040.0) || ((intSize + decimalPlaces) > 7)))
        {
            row2Display("NUMBER TOO LARGE");
        }
        else if (type == 2 && (digitsSize + 2) > 16)
        {
            row2Display("OUT OF BOUNDS");
        }
        else
        {
            row2Display(convertedValue);
        }
        delay(WT);
        allClear();
    }
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

bool checkValid(char toCheck)
{
    // RETURN 0 INVALID, 1 VALID
    if (isDigit(toCheck) || toCheck == '-' || toCheck == '.')
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
void row1Display(String toDisplayText)
{
    output = "";
    lcd.setCursor(0, 0);
    output.concat(toDisplayText);
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

void allClear()
{
    lcd.setCursor(0, 0);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print("                ");
}

float convertExtended()
{
    float nonExp = 0;
    int expo = 0;

    nonExp = convertToFloat(nonExponent);
    expo = atoi(exponent);
    tolerance = 0;

    if ((intSize + decimalPlaces) < 6)
    {
        tolerance = decimalPlaces - expo;
    }
    else
    {
        tolerance = (7 - intSize) - expo;
    }
    // CLIP NEGATIVE TOLERANCES
    if (tolerance < 0)
    {
        tolerance = 0;
    }
    // IDENTIFY IF THE NUMBER EXPANDS (DECIMAL PLACES) AND BY HOW MUCH
    int expansion = 0;
    type = 0;
    digitsSize = intSize + decimalPlaces;

    if ((expo > 0) && ((expo - decimalPlaces) >= 0))
    {
        // EXPAND LEFT
        type = 1;
        expansion = expo - decimalPlaces;
        digitsSize = intSize + decimalPlaces + expansion;
    }

    if ((expo < 0) && (((0 - expo) - intSize) >= 0))
    {
        // EXPAND RIGHT
        type = 2;
        expansion = abs((expo) + intSize);
        digitsSize = intSize + decimalPlaces + expansion + 1;
        tolerance = digitsSize - 1;
    }

    float converted = 0;
    if (expo == NULL)
    {
        // NON EXP FILTER
        tolerance = decimalPlaces;
        converted = nonExp;
    }
    else if (type == 1)
    {
        // TYPE 1 FILTER
        float nuke = 1;
        for (int i = 0; i < decimalPlaces; i++)
        {
            nonExp = nonExp * 10;
        }
        for (int i = 0; i < expo - decimalPlaces; i++)
        {
            nuke = nuke * 10;
        }
        long sohInt = (long)(nonExp + 0.5);
        converted = sohInt * nuke;
        return converted;
    }
    else if (type == 0 || type == 2)
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
        // IDENTIFICATION
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
            nonExponent[nonExponentSize] = extract[i];
            nonExponentSize++;
            break;
        case 2:
            if (!(extract[i] == 'e'))
            {
                exponent[exponentSize] = extract[i];
                exponentSize++;
            }
            else if (i == (extractSize - 1))
            {
                ignored[0] = 'e';
                partiality = 1;
            }
            break;
        case 3: // EXPONENT AND IGNORED
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
    }
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