TYPE
  LOGLEVEL : (CRITICAL, WARNING, INFO, DEBUG) := INFO;
END_TYPE

FUNCTION_BLOCK LOGGER
  VAR_INPUT
    TRIG : BOOL;
    MSG : STRING;
    LEVEL : LOGLEVEL := INFO;
  END_VAR
  VAR
    TRIG0 : BOOL;
  END_VAR

  IF TRIG AND NOT TRIG0 THEN
  {{
   LogMessage(GetFbVar(LEVEL),(char*)GetFbVar(MSG, .body),GetFbVar(MSG, .len));
  }}
  END_IF;
  TRIG0:=TRIG;
END_FUNCTION_BLOCK



FUNCTION_BLOCK python_eval
  VAR_INPUT
    TRIG : BOOL;
    CODE : STRING;
  END_VAR
  VAR_OUTPUT
    ACK : BOOL;
    RESULT : STRING;
  END_VAR
  VAR
    STATE : DWORD;
    BUFFER : STRING;
    PREBUFFER : STRING;
    TRIGM1 : BOOL;
    TRIGGED : BOOL;
  END_VAR

  {extern void __PythonEvalFB(int, PYTHON_EVAL*);__PythonEvalFB(0, data__);}
END_FUNCTION_BLOCK

FUNCTION_BLOCK python_poll
  VAR_INPUT
    TRIG : BOOL;
    CODE : STRING;
  END_VAR
  VAR_OUTPUT
    ACK : BOOL;
    RESULT : STRING;
  END_VAR
  VAR
    STATE : DWORD;
    BUFFER : STRING;
    PREBUFFER : STRING;
    TRIGM1 : BOOL;
    TRIGGED : BOOL;
  END_VAR

  {extern void __PythonEvalFB(int, PYTHON_EVAL*);__PythonEvalFB(1,(PYTHON_EVAL*)(void*)data__);}
END_FUNCTION_BLOCK

FUNCTION_BLOCK python_gear
  VAR_INPUT
    N : UINT;
    TRIG : BOOL;
    CODE : STRING;
  END_VAR
  VAR_OUTPUT
    ACK : BOOL;
    RESULT : STRING;
  END_VAR
  VAR
    py_eval : python_eval;
    COUNTER : UINT;
    _TMP_ADD10_OUT : UINT;
    _TMP_EQ13_OUT : BOOL;
    _TMP_SEL15_OUT : UINT;
    _TMP_AND7_OUT : BOOL;
  END_VAR

  _TMP_ADD10_OUT := ADD(COUNTER, 1);
  _TMP_EQ13_OUT := EQ(N, _TMP_ADD10_OUT);
  _TMP_SEL15_OUT := SEL(_TMP_EQ13_OUT, _TMP_ADD10_OUT, 0);
  COUNTER := _TMP_SEL15_OUT;
  _TMP_AND7_OUT := AND(_TMP_EQ13_OUT, TRIG);
  py_eval(TRIG := _TMP_AND7_OUT, CODE := CODE);
  ACK := py_eval.ACK;
  RESULT := py_eval.RESULT;
END_FUNCTION_BLOCK



TYPE
  HMI_REAL : REAL;
  HMI_NODE : BOOL;
  HMI_STRING : STRING;
  HMI_BOOL : BOOL;
  HMI_INT : INT;
END_TYPE


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
