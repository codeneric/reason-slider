type section;

type jsmodule;

[@bs.val] external _module : jsmodule = "module";

type story = unit => ReasonReact.reactElement;

[@bs.val] [@bs.module "@storybook/react"] external createSection : (string, jsmodule) => section =
  "storiesOf";

[@bs.send.pipe : section] external addDecorator : (story => ReasonReact.reactElement) => section =
  "";

[@bs.send.pipe : section] external addStory : (string, story) => section = "add";

module Knobs = {
  [@bs.val] [@bs.module "@storybook/addon-knobs/react"]
  external text : (string, string) => string =
    "";
  [@bs.val] [@bs.module "@storybook/addon-knobs/react"]
  external jsobject : (string, Js.t('a)) => Js.t('a) =
    "object";
  [@bs.val] [@bs.module "@storybook/addon-knobs/react"]
  external boolean : (string, Js.boolean) => Js.boolean =
    "";
  [@bs.val] [@bs.module "@storybook/addon-knobs/react"] external int : (string, int) => int =
    "number";
  [@bs.val] [@bs.module "@storybook/addon-knobs/react"] external float : (string, float) => float =
    "number";
  [@bs.val] [@bs.module "@storybook/addon-knobs/react"]
  external button : (string, unit => unit) => unit =
    "";
  [@bs.val] [@bs.module "@storybook/addon-knobs/react"]
  external withKnobs : story => ReasonReact.reactElement =
    "";
};