<?php
class OPBackStageFetchLucky extends PBMessage
{
  var $wired_type = PBMessage::WIRED_LENGTH_DELIMITED;
  public function __construct($reader=null)
  {
    parent::__construct($reader);
    $this->fields["1"] = "PBInt";
    $this->values["1"] = "";
    $this->fields["2"] = "PBString";
    $this->values["2"] = "";
  }
  function version()
  {
    return $this->_get_value("1");
  }
  function set_version($value)
  {
    return $this->_set_value("1", $value);
  }
  function name()
  {
    return $this->_get_value("2");
  }
  function set_name($value)
  {
    return $this->_set_value("2", $value);
  }
}
class OPBackStageFetchLuckyRet extends PBMessage
{
  var $wired_type = PBMessage::WIRED_LENGTH_DELIMITED;
  public function __construct($reader=null)
  {
    parent::__construct($reader);
    $this->fields["1"] = "PBInt";
    $this->values["1"] = "";
    $this->fields["2"] = "PBInt";
    $this->values["2"] = "";
    $this->fields["3"] = "PBString";
    $this->values["3"] = "";
    $this->fields["4"] = "PBInt";
    $this->values["4"] = "";
    $this->fields["5"] = "PBInt";
    $this->values["5"] = "";
    $this->fields["6"] = "PBInt";
    $this->values["6"] = "";
    $this->fields["7"] = "PBInt";
    $this->values["7"] = "";
    $this->fields["8"] = "PBInt";
    $this->values["8"] = "";
    $this->fields["9"] = "PBInt";
    $this->values["9"] = "";
    $this->fields["10"] = "PBInt";
    $this->values["10"] = "";
    $this->fields["11"] = "PBInt";
    $this->values["11"] = "";
    $this->fields["12"] = "PBInt";
    $this->values["12"] = "";
    $this->fields["13"] = "PBInt";
    $this->values["13"] = "";
    $this->fields["14"] = "PBInt";
    $this->values["14"] = "";
    $this->fields["15"] = "PBInt";
    $this->values["15"] = "";
    $this->fields["16"] = "PBInt";
    $this->values["16"] = "";
  }
  function servertime()
  {
    return $this->_get_value("1");
  }
  function set_servertime($value)
  {
    return $this->_set_value("1", $value);
  }
  function userid()
  {
    return $this->_get_value("2");
  }
  function set_userid($value)
  {
    return $this->_set_value("2", $value);
  }
  function name()
  {
    return $this->_get_value("3");
  }
  function set_name($value)
  {
    return $this->_set_value("3", $value);
  }
  function registertime()
  {
    return $this->_get_value("4");
  }
  function set_registertime($value)
  {
    return $this->_set_value("4", $value);
  }
  function lastlogin()
  {
    return $this->_get_value("5");
  }
  function set_lastlogin($value)
  {
    return $this->_set_value("5", $value);
  }
  function level()
  {
    return $this->_get_value("6");
  }
  function set_level($value)
  {
    return $this->_set_value("6", $value);
  }
  function silvercoins()
  {
    return $this->_get_value("7");
  }
  function set_silvercoins($value)
  {
    return $this->_set_value("7", $value);
  }
  function goldcoins()
  {
    return $this->_get_value("8");
  }
  function set_goldcoins($value)
  {
    return $this->_set_value("8", $value);
  }
  function thisLuckPool()
  {
    return $this->_get_value("9");
  }
  function set_thisLuckPool($value)
  {
    return $this->_set_value("9", $value);
  }
  function nextLuckPool()
  {
    return $this->_get_value("10");
  }
  function set_nextLuckPool($value)
  {
    return $this->_set_value("10", $value);
  }
  function luckOne()
  {
    return $this->_get_value("11");
  }
  function set_luckOne($value)
  {
    return $this->_set_value("11", $value);
  }
  function luckTwo()
  {
    return $this->_get_value("12");
  }
  function set_luckTwo($value)
  {
    return $this->_set_value("12", $value);
  }
  function treasureWorth()
  {
    return $this->_get_value("13");
  }
  function set_treasureWorth($value)
  {
    return $this->_set_value("13", $value);
  }
  function thisDisciple()
  {
    return $this->_get_value("14");
  }
  function set_thisDisciple($value)
  {
    return $this->_set_value("14", $value);
  }
  function nextDisciple()
  {
    return $this->_get_value("15");
  }
  function set_nextDisciple($value)
  {
    return $this->_set_value("15", $value);
  }
  function discipleLucky()
  {
    return $this->_get_value("16");
  }
  function set_discipleLucky($value)
  {
    return $this->_set_value("16", $value);
  }
}
?>