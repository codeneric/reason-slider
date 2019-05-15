module type SliderType = {
  type item;
  type pointer;
  type index = int;
  let getItem: (pointer, list(item)) => item;
  let getPointer: (item, index) => pointer;
};

module Default = {
  type index = int;
};

[%bs.raw
  "(function() {\n  var throttle = function(type, name, obj) {\n      obj = obj || window;\n      var running = false;\n      var func = function() {\n          if (running) { return; }\n          running = true;\n           requestAnimationFrame(function() {\n              obj.dispatchEvent(new CustomEvent(name));\n              running = false;\n          });\n      };\n      obj.addEventListener(type, func);\n  };\n\n  /* init - you can init any event */\n  throttle('resize', 'optimizedResize');\n})()"
];

module Make = (M: SliderType) => {
  module Calculations = {
    type layout = {
      x: int,
      y: int,
      width: int,
      height: int,
      scale: float,
    };
    type source = option(Dom.element);
    type id = string;
    /* type vector = (int, int);
       /* type action =
          | Update
          | Open(option(Dom.element))
          | Close; */
       /* | Resize
          | Close(source)
          | Navigate(id); */
       type longestEdge =
         | Width(int)
         | Height(int)
         | Equal(int);
       let getLongestOriginalEdge = ((x, y)) => x === y ? Equal(x) : x > y ? Width(x) : Height(y); */
    let getTargetInnerHeight = target =>
      switch (target) {
      | None => Webapi.Dom.Window.innerHeight(Webapi.Dom.window)
      | Some(target) =>
        target
        |> Webapi.Dom.Element.getBoundingClientRect
        |> Webapi.Dom.DomRect.height
        |> int_of_float
      };
    let getTargetInnerWidth = target =>
      switch (target) {
      | None => Webapi.Dom.Window.innerWidth(Webapi.Dom.window)
      | Some(target) =>
        target
        |> Webapi.Dom.Element.getBoundingClientRect
        |> Webapi.Dom.DomRect.width
        |> int_of_float
      };
    /* optimized resize  https://developer.mozilla.org/de/docs/Web/Events/resize */

    let getScreenCenter = target => {
      let w = getTargetInnerWidth(target);
      let h = getTargetInnerHeight(target);
      (w / 2, h / 2);
    };
    let getElementOffset = ((x, y), target) => {
      let (xCenter, yCenter) = getScreenCenter(target);
      (xCenter - x / 2, yCenter - y / 2);
    };
    /* let getlongestEdge = (target, longest) =>
         switch longest {
         | Width(width) =>
           let v = min(width, getTargetInnerWidth(target));
           (Width(v), float_of_int(v) /. float_of_int(width))
         | Height(height) =>
           let v = min(height, getTargetInnerHeight(target));
           (Height(v), float_of_int(v) /. float_of_int(height))
         | Equal(length) =>
           let v = min(length, min(getTargetInnerWidth(target), getTargetInnerHeight(target)));
           (Equal(v), float_of_int(v) /. float_of_int(length))
         };
       let scale = (x, factor) => int_of_float @@ ceil(float_of_int(x) *. factor); */
    /* let scale = (source, target) => int_of_float @@ ceil(float_of_int(x) *. scale) */
    let calculateFullscreenImageDimensions = ((originalX, originalY), target) => {
      let widthConstraint = min(originalX, getTargetInnerWidth(target));
      let heightConstraint = min(originalY, getTargetInnerHeight(target));
      let foi = float_of_int;
      let iof = int_of_float;
      let scale =
        min(
          (foi @@ widthConstraint) /. (foi @@ originalX),
          (foi @@ heightConstraint) /. (foi @@ originalY),
        );
      (
        iof @@ (foi @@ originalX) *. scale,
        iof @@ (foi @@ originalY) *. scale,
      );
      /* let (longest, scalingFactor) = getLongestOriginalEdge(dimensions) |> getlongestEdge(target);
         switch longest {
         | Width(x) => (x, scale(originalY, scalingFactor))
         | Height(y) => (scale(originalX, scalingFactor), y)
         | Equal(length) =>
           let v = scale(length, scalingFactor);
           (length, length)
         } */
    };
    let getDimension = element => {
      open Webapi.Dom;
      let rect = Element.getBoundingClientRect(element);
      DomRect.(top(rect), left(rect), width(rect), height(rect));
    };
    let precisionRound = (float: float, precision) => {
      let factor = float_of_int @@ Js.Math.pow_int(10, precision);
      Js.Math.round(float *. factor) /. factor;
    };
  };
  module Component = {
    type items = list(M.item);
    type action =
      | Close
      | Update
      | Next
      | Previous
      | Show(M.pointer)
      | ShowNavigation
      | ShowNavigationAndAutoHide
      | HideNavigation;
    let maybeExecute = (optionFn, args) =>
      switch (optionFn) {
      | None => ()
      | Some(fn) => fn(args)
      };
    let maybeBool = b =>
      switch (b |> Js.Nullable.toOption) {
      | None => false
      | Some(m) => m
      };
    let indexFromPointer = (items, pointer): M.index =>
      items
      |> Array.of_list
      |> Js.Array.findIndexi((item, i) => pointer === M.getPointer(item, i));
    let pointerFromIndex = (items, i: M.index) =>
      M.getPointer(List.nth(items, i), i);
    let nextPointer = (pointer, items) => {
      let index = pointer |> indexFromPointer(items);
      index + 1 |> pointerFromIndex(items);
    };
    let previousPointer = (pointer, items) => {
      let index = pointer |> indexFromPointer(items);
      index - 1 |> pointerFromIndex(items);
    };
    let isPointerFirstEntry = (pointer, items) =>
      pointer |> indexFromPointer(items) === 0;
    let isPointerLastEntry = (pointer, items) =>
      pointer |> indexFromPointer(items) === List.length(items) - 1;
    Js.log(Bowser.bowser);
    let isDesktop = () =>
      !(Bowser.bowser##mobile |> maybeBool)
      && !(Bowser.bowser##tablet |> maybeBool);
    type state = {
      isOpen: bool,
      showNavigation: bool,
      pointer: option(M.pointer),
    };
    module Portal = {
      [@bs.module "react-portal"]
      external portal: ReasonReact.reactClass = "Portal";
      Js.log(portal);
      let make = children =>
        ReasonReact.wrapJsForReason(
          ~reactClass=portal,
          ~props=Js.Obj.empty(),
          children,
        );
    };
    /* let getFullscreenWidth = (elementWidth, containerWidth) : float =>
       elementWidth > containerWidth ? containerWidth : elementWidth; */
    let component = ReasonReact.reducerComponent("Slider");
    let onResizeHandle = ({ReasonReact.send, state, _}, _) =>
      state.isOpen ? send(Update) : ();
    /* type openFn = (~from: option(Dom.element), ~target: id, unit) => unit; */
    /* type render = (openFn, unit) => ReasonReact.reactElement; */
    [@bs.deriving jsConverter]
    type renderProps = {
      show: M.pointer => unit,
      close: unit => unit,
    };
    module RenderSlideProps = {
      [@bs.deriving jsConverter]
      type t = {
        progress: float,
        item: M.item,
        close: unit => unit,
      };
    };
    module OnChange = {
      [@bs.deriving jsConverter]
      type t = {
        pointer: M.pointer,
        items,
        index: M.index,
      };
    };
    type renderFn = renderProps => ReasonReact.reactElement;
    type renderSlideFn = RenderSlideProps.t => ReasonReact.reactElement;
    let combokeys: ref(option(Combokeys.t)) = ref(None);
    let getCombokeys = () =>
      switch (combokeys^) {
      | None =>
        let ck =
          Combokeys.init(
            Webapi.Dom.document |> Webapi.Dom.Document.documentElement,
          );
        combokeys := Some(ck);
        ck;
      | Some(ck) => ck
      };
    let bindShortcuts = ({ReasonReact.send}) => {
      let c = getCombokeys();
      c |> Combokeys.bind("esc", _ => send(Close));
      c |> Combokeys.bind("left", _ => send(Previous));
      c |> Combokeys.bind("right", _ => send(Next));
    };
    let detachShortcuts = () => {
      let c = getCombokeys();
      c |> Combokeys.detach();
      combokeys := None;
    };
    let disableScrollFn = () =>
      [%bs.raw {|  document.body.style.overflow = "hidden"   |}] |> ignore;
    let enableScrollFn = () =>
      [%bs.raw {|  document.body.style.overflow = "initial"   |}] |> ignore;
    let callOnChange = ({ReasonReact.state, _}, items, onChange) =>
      switch (onChange, state.pointer) {
      | (Some(fn), Some(pointer)) =>
        fn({
          OnChange.items,
          OnChange.pointer,
          OnChange.index: indexFromPointer(items, pointer),
        })
      | (_, _) => ()
      };
    let make =
        (
          ~items: items,
          ~renderSlide: renderSlideFn,
          ~onChange: option(OnChange.t => unit)=?,
          ~render: renderFn,
          ~hideNavigationTimeout: int=2000,
          ~backgroundColor: string="rgba(255,255,255,1)",
          ~disableScroll: bool=true,
          ~zIndex: int=1000,
          _children,
        ) => {
      let renderNavigation = ({ReasonReact.send, state, _}) => {
        let enabledFill = "rgba(0,0,0,0.8)";
        let disabledFill = "rgba(0,0,0,0.25)";
        let getFill = fn =>
          switch (state.pointer) {
          | None => disabledFill
          | Some(p) => fn(p, items) ? disabledFill : enabledFill
          };
        let arrow = (fn, rotate) =>
          <svg
            width="64"
            height="64"
            fill={getFill(fn)}
            version="1"
            transform={"rotate(" ++ string_of_int(rotate) ++ ")"}
            xmlns="http://www.w3.org/2000/svg"
            viewBox="0 0 129 129">
            <path
              d="M40 121l-3 1c-1 1-2 0-2-1-2-1-2-4 0-6l51-50-51-51a4 4 0 0 1 5-6l54 54c2 1 2 4 0 5l-54 54z"
            />
          </svg>;
        <Motion.New.TransitionMotion
          willLeave={_ =>
            Some(Js.Dict.fromList([("progress", Motion.New.spring(0))]))
            |> Js.Nullable.fromOption
          }
          willEnter={_ => Js.Dict.fromList([("progress", 0.)])}
          styles={
            state.showNavigation
              ? [|
                {
                  "style":
                    Js.Dict.fromList([
                      (
                        "progress",
                        Motion.New.springWithConfig(
                          1,
                          {
                            "stiffness": 300.,
                            "damping": 20.,
                            "precision": 0.1,
                          },
                        ),
                      ),
                    ]),
                  "key": "arrows",
                  "data": None |> Js.Nullable.fromOption,
                },
              |]
              : [||]
          }>
          ...{i =>
            switch (i) {
            | [||] => ReasonReact.null
            | i =>
              let _progress =
                Calculations.precisionRound(
                  Js.Dict.unsafeGet(i[0]##style, "progress"),
                  3,
                );
              <div>
                <div
                  onClick={_ => send(Next)}
                  disabled={
                    switch (state.pointer) {
                    | None => true
                    | Some(p) => isPointerLastEntry(p, items)
                    }
                  }
                  style={ReactDOMRe.Style.make(
                    ~position="absolute",
                    ~right="0",
                    ~width="10%",
                    ~top="0",
                    ~height=" 100%",
                    ~display="flex",
                    ~justifyContent="center",
                    ~zIndex=string_of_int(zIndex + 1),
                    ~alignItems="center",
                    (),
                  )}>
                  {arrow(isPointerLastEntry, 0)}
                </div>
                <div
                  onClick={_ => send(Previous)}
                  disabled={
                    switch (state.pointer) {
                    | None => true
                    | Some(p) => isPointerFirstEntry(p, items)
                    }
                  }
                  style={ReactDOMRe.Style.make(
                    ~position="absolute",
                    ~left="0",
                    ~width="10%",
                    ~top=" 0",
                    ~height=" 100%",
                    ~display="flex",
                    ~justifyContent="center",
                    ~zIndex=string_of_int(zIndex + 1),
                    ~alignItems="center",
                    (),
                  )}>
                  {arrow(isPointerFirstEntry, 180)}
                </div>
              </div>;
            }
          }
        </Motion.New.TransitionMotion>;
      };
      {
        ...component,
        initialState: () => {
          isOpen: false,
          pointer: None,
          showNavigation:
            /*** TODO: put this as prop */
            isDesktop(),
        },
        didMount: self => {
          Webapi.Dom.Window.addEventListener(
            "optimizedResize",
            onResizeHandle(self),
            Webapi.Dom.window,
          );
          self.onUnmount(() =>
            Webapi.Dom.Window.removeEventListener(
              "optimizedResize",
              onResizeHandle(self),
              Webapi.Dom.window,
            )
          );
        },
        reducer: (action: action, state: state) =>
          switch (action) {
          | Update => ReasonReact.Update(state)
          | Show(pointer) =>
            ReasonReact.UpdateWithSideEffects(
              {...state, isOpen: true, pointer: Some(pointer)},
              self => {
                bindShortcuts(self);
                if (disableScroll) {
                  disableScrollFn();
                };
                callOnChange(self, items, onChange);
                ();
              },
            )
          | Next =>
            !state.isOpen
              ? ReasonReact.NoUpdate
              : (
                switch (state.pointer) {
                | None => ReasonReact.NoUpdate
                | Some(pointer) =>
                  let nextIndex = (pointer |> indexFromPointer(items)) + 1;
                  nextIndex >= List.length(items)
                    ? ReasonReact.NoUpdate
                    : ReasonReact.UpdateWithSideEffects(
                        {
                          ...state,
                          pointer: Some(nextPointer(pointer, items)),
                        },
                        self => callOnChange(self, items, onChange),
                      );
                }
              )
          | Previous =>
            !state.isOpen
              ? ReasonReact.NoUpdate
              : (
                switch (state.pointer) {
                | None => ReasonReact.NoUpdate
                | Some(pointer) =>
                  let prevIndex = (pointer |> indexFromPointer(items)) - 1;
                  prevIndex < 0
                    ? ReasonReact.NoUpdate
                    : ReasonReact.UpdateWithSideEffects(
                        {
                          ...state,
                          pointer: Some(previousPointer(pointer, items)),
                        },
                        self => callOnChange(self, items, onChange),
                      );
                }
              )
          | Close =>
            ReasonReact.UpdateWithSideEffects(
              {...state, isOpen: false, pointer: None},
              _ => {
                if (disableScroll) {
                  enableScrollFn();
                };
                detachShortcuts();
              },
            )
          | ShowNavigation =>
            ReasonReact.Update({...state, showNavigation: true})
          | ShowNavigationAndAutoHide =>
            ReasonReact.UpdateWithSideEffects(
              {...state, showNavigation: true},
              ({send, _}) =>
                Js.Global.setTimeout(
                  () => send(HideNavigation),
                  hideNavigationTimeout,
                )
                |> ignore,
            )
          | HideNavigation =>
            ReasonReact.Update({...state, showNavigation: false})
          },
        /* subscriptions: self =>
           Webapi.Dom.
             /* Sub(
                  () => {
                    combokeys |> Combokeys.bind("esc", (_) => self.send(Close));
                    combokeys |> Combokeys.bind("left", (_) => self.send(Previous));
                    combokeys |> Combokeys.bind("right", (_) => self.send(Next))
                  },
                  () => combokeys |> Combokeys.detach()
                ), */
             [
               Sub(
                 () =>
                   Window.addEventListener(
                     "optimizedResize",
                     onResizeHandle(self),
                     window,
                   ),
                 /* () =>
                    Window.removeEventListener(
                      "optimizedResize",
                      onResizeHandle(self),
                      window,
                    ), */
               ),
             ], */
        render: ({state, send, _} as self) => {
          let closeFn = () => send(Close);
          <div>
            <Motion.New.TransitionMotion
              willLeave={_ =>
                Some(
                  Js.Dict.fromList([("progress", Motion.New.spring(0))]),
                )
                |> Js.Nullable.fromOption
              }
              willEnter={_ => Js.Dict.fromList([("progress", 0.)])}
              styles={
                state.isOpen
                  ? [|
                    {
                      "style":
                        Js.Dict.fromList([
                          ("progress", Motion.New.spring(1)),
                        ]),
                      "key": "slider",
                      "data": None |> Js.Nullable.fromOption,
                    },
                  |]
                  : [||]
              }>
              ...{i =>
                switch (i) {
                | [||] => ReasonReact.null
                | i =>
                  let progress =
                    Calculations.precisionRound(
                      Js.Dict.unsafeGet(i[0]##style, "progress"),
                      3,
                    );
                  <Portal>
                    <div
                      onClick={_ => send(Close)}
                      style={ReactDOMRe.Style.make(
                        ~zIndex=string_of_int(zIndex),
                        ~position="fixed",
                        ~top="0",
                        ~left="0",
                        ~pointerEvents=progress <= 0.95 ? "none" : "auto",
                        ~opacity=Js.Float.toString(progress),
                        ~backgroundColor,
                        ~width="100%",
                        ~height="100%",
                        (),
                      )}>
                      {progress >= 0.8
                         ? <div
                             onClick={e => {
                               ReactEvent.Mouse.stopPropagation(e);
                               if (!isDesktop() && !state.showNavigation) {
                                 send(ShowNavigationAndAutoHide);
                               };
                             }}>
                             {renderNavigation(self)}
                             {switch (state.pointer) {
                              | None => ReasonReact.null
                              | Some(p) =>
                                try (
                                  renderSlide({
                                    progress,
                                    item: M.getItem(p, items),
                                    close: closeFn,
                                  })
                                ) {
                                | _ =>
                                  send(Close);
                                  ReasonReact.null;
                                }
                              }}
                           </div>
                         : ReasonReact.null}
                    </div>
                  </Portal>;
                }
              }
            </Motion.New.TransitionMotion>
            {render({show: p => send(Show(p)), close: closeFn})}
          </div>;
        },
      };
    };
  };
};