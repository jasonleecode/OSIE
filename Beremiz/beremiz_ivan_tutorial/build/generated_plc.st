FUNCTION_BLOCK CounterST
  VAR_INPUT
    Reset : BOOL;
  END_VAR
  VAR_OUTPUT
    Out : INT;
  END_VAR
  VAR
    Cnt : INT;
  END_VAR
  VAR_EXTERNAL
    ResetCounterValue : INT;
    Relay0Value : BOOL;
  END_VAR

  IF Reset THEN
    Cnt := ResetCounterValue;
  ELSE
    Cnt := Cnt + 1;
  END_IF;

  Out := Cnt;
END_FUNCTION_BLOCK

PROGRAM plc_prg
  VAR_INPUT
    Reset : BOOL;
  END_VAR
  VAR_OUTPUT
    Cnt0 : INT;
  END_VAR
  VAR
    CounterST0 : CounterST;
  END_VAR
  VAR
    Relay0Value AT %QX0.0.0.0 : BOOL;
  END_VAR

  CounterST0(Reset := Reset);
  Cnt0 := CounterST0.Out;
END_PROGRAM


CONFIGURATION config
  VAR_GLOBAL
    ResetCounterValue : INT := 0;
    Relay0Value AT %QX0.0.0.0 : BOOL := 0;
    Relay1Value AT %QX0.0.1.1 : BOOL := 0;
    Relay2Value AT %QX0.0.2.2 : BOOL := 0;
    Relay3Value AT %QX0.0.3.3 : BOOL := 0;
  END_VAR

  RESOURCE resource1 ON PLC
    TASK task0(INTERVAL := T#20ms,PRIORITY := 0);
    PROGRAM instance0 WITH task0 : plc_prg;
  END_RESOURCE
END_CONFIGURATION
