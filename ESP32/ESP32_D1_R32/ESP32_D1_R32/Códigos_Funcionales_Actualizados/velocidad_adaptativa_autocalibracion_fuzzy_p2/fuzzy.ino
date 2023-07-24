
void set_fuzzy()
{
  // Every setup must occur in the function setup()

  // FuzzyInput
  FuzzyInput *error_abs = new FuzzyInput(1);

  error_abs->addFuzzySet(pequenno);
  error_abs->addFuzzySet(mediano);
  error_abs->addFuzzySet(grande);
  fuzzy->addFuzzyInput(error_abs);

  // FuzzyInput
  FuzzyInput *curvatura_abs = new FuzzyInput(2);

  curvatura_abs->addFuzzySet(recta);
  curvatura_abs->addFuzzySet(curva_suave);
  curvatura_abs->addFuzzySet(curva_cerrada);
  fuzzy->addFuzzyInput(curvatura_abs);


  // FuzzyOutput
  FuzzyOutput *controlador = new FuzzyOutput(1);

  controlador->addFuzzySet(doble);
  controlador->addFuzzySet(doble_ponderado);
  controlador->addFuzzySet(simple);
  fuzzy->addFuzzyOutput(controlador);

  // construyendo regla 1
  FuzzyRuleAntecedent *si_curva_cerrada_AND_error_pequenno = new FuzzyRuleAntecedent();
  si_curva_cerrada_AND_error_pequenno->joinWithAND(curva_cerrada, pequenno);

  FuzzyRuleConsequent *entonces_controlador_doblePonderado = new FuzzyRuleConsequent();
  entonces_controlador_doblePonderado->addOutput(doble_ponderado);

  FuzzyRule *fuzzyRule1 = new FuzzyRule(1, si_curva_cerrada_AND_error_pequenno, entonces_controlador_doblePonderado);
  fuzzy->addFuzzyRule(fuzzyRule1);

  // construyendo regla 2
  FuzzyRuleAntecedent *error_mediano_OR_grande = new FuzzyRuleAntecedent();
  error_mediano_OR_grande->joinWithOR(mediano, grande);
  FuzzyRuleAntecedent *curvaCerrada = new FuzzyRuleAntecedent();
  curvaCerrada->joinSingle(curva_cerrada);
  FuzzyRuleAntecedent *si_curva_cerrada_AND_error_mediano_OR_grande = new FuzzyRuleAntecedent();
  si_curva_cerrada_AND_error_mediano_OR_grande->joinWithAND(curvaCerrada, error_mediano_OR_grande);

  FuzzyRuleConsequent *entonces_controlador_simple = new FuzzyRuleConsequent();
  entonces_controlador_simple->addOutput(simple);

  FuzzyRule *fuzzyRule2 = new FuzzyRule(2, si_curva_cerrada_AND_error_mediano_OR_grande, entonces_controlador_simple);
  fuzzy->addFuzzyRule(fuzzyRule2);

  // construyendo regla 3
  FuzzyRuleAntecedent *si_curva_suave_AND_error_pequenno = new FuzzyRuleAntecedent();
  si_curva_suave_AND_error_pequenno->joinWithAND(curva_suave, pequenno);

  FuzzyRuleConsequent *entonces_controlador_doble = new FuzzyRuleConsequent();
  entonces_controlador_doble->addOutput(doble);

  FuzzyRule *fuzzyRule3 = new FuzzyRule(3, si_curva_suave_AND_error_pequenno, entonces_controlador_doble);
  fuzzy->addFuzzyRule(fuzzyRule3);

  // construyendo regla 4
  FuzzyRuleAntecedent *si_curva_suave_AND_error_mediano = new FuzzyRuleAntecedent();
 si_curva_suave_AND_error_mediano->joinWithAND(curva_suave, mediano);

  FuzzyRule *fuzzyRule4 = new FuzzyRule(4, si_curva_suave_AND_error_mediano, entonces_controlador_doblePonderado);
  fuzzy->addFuzzyRule(fuzzyRule4);

  // construyendo regla 5
  FuzzyRuleAntecedent *si_curva_suave_AND_error_grande = new FuzzyRuleAntecedent();
  si_curva_suave_AND_error_grande->joinWithAND(curva_suave, grande);


  FuzzyRule *fuzzyRule5 = new FuzzyRule(5, si_curva_suave_AND_error_grande, entonces_controlador_simple);
  fuzzy->addFuzzyRule(fuzzyRule5);

  // construyendo regla 6
  FuzzyRuleAntecedent *si_recta = new FuzzyRuleAntecedent();
  si_recta->joinSingle(recta);


  FuzzyRule *fuzzyRule6 = new FuzzyRule(6, si_recta, entonces_controlador_doble);
  fuzzy->addFuzzyRule(fuzzyRule6);
}
