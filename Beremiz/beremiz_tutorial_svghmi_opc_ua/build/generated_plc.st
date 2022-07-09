FUNCTION_BLOCK CounterST
  VAR
    Cnt0 : INT;
  END_VAR
  VAR_EXTERNAL
    Relay0 : DINT;
    Relay1 : DINT;
    Relay2 : DINT;
    Relay3 : DINT;
    HMI_RELAY0 : HMI_INT;
    HMI_RELAY1 : HMI_INT;
    HMI_RELAY2 : HMI_INT;
    HMI_RELAY3 : HMI_INT;
  END_VAR

  IF HMI_RELAY2 >= 1 THEN
    Relay2:=1;
  ELSE
    Relay2:=0;
  END_IF;

  IF HMI_RELAY3 >= 1 THEN
    Relay3:=1;
  ELSE
    Relay3:=0;
  END_IF;

  Cnt0 := Cnt0 + 1;
  IF Cnt0 = 50 THEN
    Relay0 := 1;
    HMI_RELAY0:= 1;
    Relay1 := 1;
    HMI_RELAY1:= 1;
  END_IF;
    
  IF Cnt0 = 100 THEN
    Relay0 := 0;
    HMI_RELAY0:= 0;
    Relay1 := 0;
    HMI_RELAY1:= 0;
    Cnt0 := 0;
  END_IF;
    
END_FUNCTION_BLOCK

PROGRAM plc_prg
  VAR
    CounterST0 : CounterST;
  END_VAR

  CounterST0();
END_PROGRAM


CONFIGURATION config
  VAR_GLOBAL
    Relay0 AT %QD1.0 : DINT := 0;
    Relay1 AT %QD1.1 : DINT := 0;
    Relay2 AT %QD1.2 : DINT := 0;
    Relay3 AT %QD1.3 : DINT := 0;
    HMI_RELAY0 : HMI_INT := 0;
    HMI_RELAY1 : HMI_INT := 0;
    HMI_RELAY2 : HMI_INT := 0;
    HMI_RELAY3 : HMI_INT := 0;
    HMI_ROOT : HMI_NODE;
    heartbeat : HMI_INT;
    CURRENT_PAGE_0 : HMI_STRING;
  END_VAR

  RESOURCE resource1 ON PLC
    TASK task0(INTERVAL := T#20ms,PRIORITY := 0);
    PROGRAM instance0 WITH task0 : plc_prg;
  END_RESOURCE
END_CONFIGURATION
