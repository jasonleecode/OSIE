FUNCTION_BLOCK CounterST
  VAR_INPUT
    Reset : BOOL;
  END_VAR
  VAR_OUTPUT
    Out1 : INT;
    Out0 : INT;
  END_VAR
  VAR
    Cnt1 : INT;
    Cnt0 : INT;
  END_VAR
  VAR_EXTERNAL
    ResetCounterValue : INT;
    Relay0Value : BOOL;
    Relay1Value : BOOL;
    Relay2Value : BOOL;
    Relay3Value : BOOL;
    Relay3Value0 : DINT;
    Relay3Value1 : DINT;
    Relay3Value2 : DINT;
    Relay3Value3 : DINT;
  END_VAR

  IF Reset THEN
    Cnt0 := ResetCounterValue;
    Cnt1 := ResetCounterValue;
    Reset := False;
  ELSE
    Cnt0 := Cnt0 + 1;
    Cnt1 := Cnt1 + 1;
    IF Cnt1 = 50 THEN
      Relay3Value0 := 1;
      Relay3Value1 := 1; 
      Relay3Value2 := 1;
      Relay3Value3 := 1;
    END_IF;
    
    IF Cnt1 = 100 THEN
      Relay3Value0 := 0;
      Relay3Value1 := 0;
      Relay3Value2 := 0;
      Relay3Value3 := 0;
      Cnt1 := 0;
    END_IF;
    
  END_IF;

  Out1 := Cnt1;
  Out0 := Cnt0;
END_FUNCTION_BLOCK

PROGRAM plc_prg
  VAR_INPUT
    Reset : BOOL;
  END_VAR
  VAR_OUTPUT
    Cnt0 : INT;
    Cnt1 : INT;
  END_VAR
  VAR
    CounterST0 : CounterST;
  END_VAR

  CounterST0(Reset := Reset);
  Cnt1 := CounterST0.Out1;
  Cnt0 := CounterST0.Out0;
END_PROGRAM


CONFIGURATION config
  VAR_GLOBAL
    ResetCounterValue : INT := 0;
    Relay0Value AT %QX0.0.0.0 : BOOL := 0;
    Relay1Value AT %QX0.0.1.1 : BOOL := 0;
    Relay2Value AT %QX0.0.2.2 : BOOL := 0;
    Relay3Value AT %QX0.0.3.3 : BOOL := 0;
    Relay3Value0 AT %QD1.0 : DINT := 0;
    Relay3Value1 AT %QD1.1 : DINT := 0;
    Relay3Value2 AT %QD1.2 : DINT := 0;
    Relay3Value3 AT %QD1.3 : DINT := 0;
  END_VAR

  RESOURCE resource1 ON PLC
    TASK task0(INTERVAL := T#20ms,PRIORITY := 0);
    PROGRAM instance0 WITH task0 : plc_prg;
  END_RESOURCE
END_CONFIGURATION
