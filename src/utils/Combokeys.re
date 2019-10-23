type t;

[@bs.new] [@bs.module] external init : Dom.element => t = "combokeys";

[@bs.send.pipe : t] external bind : (string, Dom.keyboardEvent => unit) => unit = "";

[@bs.send.pipe : t] external bindArray : (array(string), Dom.keyboardEvent => unit) => unit =
  "bind";

[@bs.send.pipe : t] external detach : unit => unit = "";
