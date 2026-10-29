<?php
class OPRecharge extends PBMessage
{
  var $wired_type = PBMessage::WIRED_LENGTH_DELIMITED;
  public function __construct($reader=null)
  {
    parent::__construct($reader);
    $this->fields["1"] = "PBString";
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
  }
  function ConsumeStreamId()
  {
    return $this->_get_value("1");
  }
  function set_ConsumeStreamId($value)
  {
    return $this->_set_value("1", $value);
  }
  function CooOrderSerial()
  {
    return $this->_get_value("2");
  }
  function set_CooOrderSerial($value)
  {
    return $this->_set_value("2", $value);
  }
  function Uin()
  {
    return $this->_get_value("3");
  }
  function set_Uin($value)
  {
    return $this->_set_value("3", $value);
  }
  function GoodsId()
  {
    return $this->_get_value("4");
  }
  function set_GoodsId($value)
  {
    return $this->_set_value("4", $value);
  }
  function GoodsCount()
  {
    return $this->_get_value("5");
  }
  function set_GoodsCount($value)
  {
    return $this->_set_value("5", $value);
  }
  function CreateTime()
  {
    return $this->_get_value("6");
  }
  function set_CreateTime($value)
  {
    return $this->_set_value("6", $value);
  }
}
class OPRechargeRet extends PBMessage
{
  var $wired_type = PBMessage::WIRED_LENGTH_DELIMITED;
  public function __construct($reader=null)
  {
    parent::__construct($reader);
    $this->fields["1"] = "PBInt";
    $this->values["1"] = "";
  }
  function status()
  {
    return $this->_get_value("1");
  }
  function set_status($value)
  {
    return $this->_set_value("1", $value);
  }
}
?>