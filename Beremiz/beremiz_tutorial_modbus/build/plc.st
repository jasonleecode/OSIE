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
  END_VAR

  IF Reset THEN
    Cnt0 := ResetCounterValue;
    Cnt1 := ResetCounterValue;
    Reset := False;
  ELSE
    Cnt0 := Cnt0 + 1;
    Cnt1 := Cnt1 + 1;
    IF Cnt1 > 10 THEN
      Relay0Value := True;
      Relay1Value := True;
    ELSE
      Relay0Value := True;
      Relay1Value := False;
    END_IF;
    
    IF Cnt1 > 15 THEN
      Relay1Value := False;
      Cnt1 :=0;
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
  END_VAR

  RESOURCE resource1 ON PLC
    TASK task0(INTERVAL := T#20ms,PRIORITY := 0);
    PROGRAM instance0 WITH task0 : plc_prg;
  END_RESOURCE
END_CONFIGURATION