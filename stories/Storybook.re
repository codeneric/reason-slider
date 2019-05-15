type module_;

type story;

type addon;

[@bs.module "@storybook/react"]
external make: (string, module_) => story = "storiesOf";

[@bs.send]
external add: (story, string, unit => ReasonReact.reactElement) => story =
  "add";
[@bs.send] external addDecorator: (story, addon) => story = "";

module Addons = {
  [@bs.module "@storybook/addon-centered/react"]
  external centered: addon = "default";
  module Knobs = {
    [@bs.module "@storybook/addon-knobs"] external withKnobs: addon = "";
    [@bs.module "@storybook/addon-knobs"]
    external boolean: (string, bool) => bool = "";
    [@bs.module "@storybook/addon-knobs"]
    external number: (string, int) => int = "";
  };
};