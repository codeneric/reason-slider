open Storyook;

module GallerySlide = {
  include Slider.Default;
  type item = {
    src: string,
    id: string,
    height: int,
    width: int,
  };
  type pointer = string;
  let getPointer = (item: item, _) => item.id;
  let getItem = (p, items) => items |> List.find(item => item.id === p);
};

module GallerySlider = Slider.Make(GallerySlide);

createSection("Slider", _module)
|> addDecorator(Knobs.withKnobs)
|> addStory("test", () => {
     let items =
       GallerySlide.[
         {
           id: "safora",
           src: "https://dummyimage.com/100x100/1abc9c/fff",
           width: 100,
           height: 100,
         },
         {
           id: "brasil",
           src: "https://dummyimage.com/500x500/1abc9c/fff",
           width: 500,
           height: 500,
         },
         {
           id: "soller",
           src: "https://dummyimage.com/1000x1000/1abc9c/fff",
           width: 1000,
           height: 1000,
         },
         {
           id: "lisboa",
           src: "https://dummyimage.com/2000x2000/1abc9c/fff",
           width: 2000,
           height: 2000,
         },
         {
           id: "lisboa3",
           src: "https://dummyimage.com/5000x1000/1abc9c/fff",
           width: 5000,
           height: 1000,
         },
         {
           id: "lisboa2",
           src: "https://dummyimage.com/1000x5000/1abc9c/fff",
           width: 1000,
           height: 5000,
         },
         {
           id: "lisboa4",
           src: "https://dummyimage.com/3000x3000/1abc9c/fff",
           width: 3000,
           height: 3000,
         },
         {
           id: "lisboa5",
           src: "https://dummyimage.com/2000x1800/1abc9c/fff",
           width: 2000,
           height: 1800,
         },
       ];
     GallerySlider.(
       <Component
         items
         onChange=(c => Js.log2("change", c))
         renderSlide=(
           ({GallerySlider.Component.RenderSlideProps.item}) => {
             let image = item;
             let target =
               Webapi.Dom.Document.getElementById(
                 "slider-content",
                 Webapi.Dom.document,
               );
             let (width, height) =
               Calculations.calculateFullscreenImageDimensions(
                 (image.width, image.height),
                 target,
               );
             /* let width = 500;
                let height = 500; */
             <div
               style=(
                 ReactDOMRe.Style.make(
                   ~display="flex",
                   ~flexDirection="column",
                   ~position="absolute",
                   ~height="100%",
                   ~width="100%",
                   (),
                 )
               )>
               <div style=(ReactDOMRe.Style.make(~height="100px", ()))>
                 (ReasonReact.string(image.id))
               </div>
               <div
                 id="slider-content"
                 style=(
                   ReactDOMRe.Style.make(
                     ~display="flex",
                     ~flexDirection="column",
                     ~flexGrow="1",
                     ~alignItems="center",
                     ~justifyContent="center",
                     ~position="relative",
                     (),
                   )
                 )>
                 <div>
                   <img
                     style=(
                       ReactDOMRe.Style.make(
                         ~width=string_of_int(width) ++ "px",
                         ~height=string_of_int(height) ++ "px",
                         ~verticalAlign="bottom",
                         (),
                       )
                     )
                     src=image.src
                   />
                 </div>
               </div>
               <div style=(ReactDOMRe.Style.make(~height="25px", ())) />
             </div>;
           }
         )
         render=(
           ({show}) =>
             <img
               onClick=(e => show("lisboa"))
               src="https://dummyimage.com/200x200/1abc9c/fff"
             />
         )
       />
     );
   });