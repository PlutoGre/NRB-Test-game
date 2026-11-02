<?php
class OPBackStageUpdateUserInfo extends PBMessage
{
  var $wired_type = PBMessage::WIRED_LENGTH_DELIMITED;
  public function __construct($reader=null)
  {
    parent::__construct($reader);
    $this->fields["1"] = "PBInt";
    $this->values["1"] = "";
    $this->fields["2"] = "PBString";
    $this->values["2"] = "";
    $this->fields["3"] = "PBString";
    $this->values["3"] = "";
    $this->fields["4"] = "PBInt";
    $this->values["4"] = "";
    $this->fields["5"] = "PBInt";
    $this->values["5"] = "";
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
  function changeType()
  {
    return $this->_get_value("3");
  }
  function set_changeType($value)
  {
    return $this->_set_value("3", $value);
  }
  function changeNum1()
  {
    return $this->_get_value("4");
  }
  function set_changeNum1($value)
  {
    return $this->_set_value("4", $value);
  }
  function changeNum2()
  {
    return $this->_get_value("5");
  }
  function set_changeNum2($value)
  {
    return $this->_set_value("5", $value);
  }
}
class OPBackStageUpdateUserInfoRet extends PBMessage
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
}
?>