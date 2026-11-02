<?php
class OPBackStagePushRewardInfo extends PBMessage
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
    $this->fields["4"] = "PBString";
    $this->values["4"] = "";
    $this->fields["5"] = "PBInt";
    $this->values["5"] = "";
    $this->fields["6"] = "PBString";
    $this->values["6"] = "";
    $this->fields["7"] = "PBInt";
    $this->values["7"] = "";
    $this->fields["8"] = "PBInt";
    $this->values["8"] = "";
    $this->fields["9"] = "PBInt";
    $this->values["9"] = "";
    $this->fields["10"] = "PBInt";
    $this->values["10"] = "";
  }
  function version()
  {
    return $this->_get_value("1");
  }
  function set_version($value)
  {
    return $this->_set_value("1", $value);
  }
  function rewardname()
  {
    return $this->_get_value("2");
  }
  function set_rewardname($value)
  {
    return $this->_set_value("2", $value);
  }
  function rewardcontent()
  {
    return $this->_get_value("3");
  }
  function set_rewardcontent($value)
  {
    return $this->_set_value("3", $value);
  }
  function field()
  {
    return $this->_get_value("4");
  }
  function set_field($value)
  {
    return $this->_set_value("4", $value);
  }
  function logintime()
  {
    return $this->_get_value("5");
  }
  function set_logintime($value)
  {
    return $this->_set_value("5", $value);
  }
  function names()
  {
    return $this->_get_value("6");
  }
  function set_names($value)
  {
    return $this->_set_value("6", $value);
  }
  function levelStart()
  {
    return $this->_get_value("7");
  }
  function set_levelStart($value)
  {
    return $this->_set_value("7", $value);
  }
  function levelEnd()
  {
    return $this->_get_value("8");
  }
  function set_levelEnd($value)
  {
    return $this->_set_value("8", $value);
  }
  function rankStart()
  {
    return $this->_get_value("9");
  }
  function set_rankStart($value)
  {
    return $this->_set_value("9", $value);
  }
  function rankEnd()
  {
    return $this->_get_value("10");
  }
  function set_rankEnd($value)
  {
    return $this->_set_value("10", $value);
  }
}
class OPBackStagePushRewardInfoRet extends PBMessage
{
  var $wired_type = PBMessage::WIRED_LENGTH_DELIMITED;
  public function __construct($reader=null)
  {
    parent::__construct($reader);
    $this->fields["1"] = "PBInt";
    $this->values["1"] = "";
  }
  function servertime()
  {
    return $this->_get_value("1");
  }
  function set_servertime($value)
  {
    return $this->_set_value("1", $value);
  }
}
?>