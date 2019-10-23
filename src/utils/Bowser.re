type t = {
  .
  "mobile": Js.Nullable.t(bool),
  "tablet": Js.Nullable.t(bool),
};

[@bs.module] external bowser : t = "bowser";
