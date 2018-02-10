/* type config;

   [@bs.module "react-motion"] external spring : int => config = "spring";

   type style = Js.Dict.t(config);

   [@bs.module "react-motion"] external comp : ReasonReact.reactClass = "Motion";

   let make = (~style: style, children: Js.Dict.t(float) => ReasonReact.reactElement) =>
     ReasonReact.wrapJsForReason(~reactClass=comp, ~props={"style": style}, children);

   module Transition = {
     type transitionStyle('data) = {. "key": string, "style": config, "data": 'data};
   }; */
module New = {
  type springConfig = {. "stiffness": float, "damping": float, "precision": float};
  type opaqueconfig;
  [@bs.module "react-motion"] external spring : int => opaqueconfig = "spring";
  [@bs.module "react-motion"] external springWithConfig : (int, springConfig) => opaqueconfig =
    "spring";
  type style = Js.Dict.t(opaqueconfig);
  type plainStyle = Js.Dict.t(float);
  module Motion = {
    [@bs.module "react-motion"] external comp : ReasonReact.reactClass = "Motion";
    let make =
        (
          ~defaultStyle: option(plainStyle)=?,
          ~style: style,
          children: plainStyle => ReasonReact.reactElement
        ) =>
      ReasonReact.wrapJsForReason(
        ~reactClass=comp,
        ~props={"style": style, "defaultStyle": defaultStyle |> Js.Nullable.from_opt},
        children
      );
  };
  module TransitionMotion = {
    type transitionStyle('data) = {. "key": string, "style": style, "data": Js.Nullable.t('data)};
    type transitionPlainStyle('data) = {
      .
      "key": string, "style": plainStyle, "data": Js.Nullable.t('data)
    };
    type willEnter('data) = transitionStyle('data) => plainStyle;
    type willLeave('data) = transitionStyle('data) => Js.Nullable.t(style);
    [@bs.module "react-motion"] external comp : ReasonReact.reactClass = "TransitionMotion";
    let make =
        (
          ~defaultStyles: option(array(transitionPlainStyle('data)))=?,
          ~willEnter: option(willEnter('data))=?,
          ~willLeave: option(willLeave('data))=?,
          ~styles: array(transitionStyle('data)),
          children: array(transitionPlainStyle('data)) => ReasonReact.reactElement
        ) =>
      ReasonReact.wrapJsForReason(
        ~reactClass=comp,
        ~props={
          "styles": styles,
          "defaultStyles": defaultStyles |> Js.Nullable.from_opt,
          "willEnter": willEnter |> Js.Nullable.from_opt,
          "willLeave": willLeave |> Js.Nullable.from_opt
        },
        children
      );
  };
};