open Utils;

type state = {
  loading: bool,
  blinds: list(blind),
  deleting: bool,
};

type action =
  | SetLoading(bool)
  | SetBlinds(list(blind))
  | AddOrderToBlind(blind, order)
  | EditOrderInBlind(blind, order)
  | RemoveOrderFromBlind(blind, order)
  | SetDeleting(bool)
;

let createReducer = (state, action) =>
  switch (action) {
  | SetBlinds(blinds) => { ...state, blinds }
  | SetLoading(loading) => { ...state, loading }
  | EditOrderInBlind(blind, order) => {
      let newBlinds = state.blinds|>List.map(testBlind =>
        testBlind.name == blind.name ? ({
          ...blind,
          orders: blind.orders |> List.map((testOrder) =>
            testOrder.id == order.id ? order : testOrder
          )
        }) : testBlind);

      {
        ...state,
        blinds: newBlinds,
      };
    }
  | AddOrderToBlind(blind, order) => {
      let newBlind = {
        ...blind,
        orders: List.concat([blind.orders, [order]]),
      };

      let newBlinds = state.blinds |> List.map(testBlind =>
        testBlind.name == blind.name ? newBlind : testBlind);

      {
        ...state,
        blinds: newBlinds,
      };
    }
  | SetDeleting(deleting) => { ...state, deleting }
  | RemoveOrderFromBlind(blind, order) => {
      let newBlind = {
        ...blind,
        orders: blind.orders |> List.filter(testOrder =>
          !(testOrder.id == order.id))
      };

      let newBlinds = state.blinds |> List.map(testBlind =>
        testBlind.name == blind.name ? newBlind : testBlind);

      {
        ...state,
        blinds: newBlinds,
      };
    }
  };

let initialState = {
  loading: true,
  blinds: [],
  deleting: false,
};

let renderBlind = (
  index,
  blind,
  onOrderEdit,
  onOrderAdd,
  onRequestDelete,
  onOrderDelete,
  deleting
) =>
  <Blind
    key=("blind-" ++ string_of_int(index))
    blind
    onOrderEdit
    onOrderAdd
    onRequestDelete
    onOrderDelete
    deleting
  />;

[@react.component]
let make = () => {
  let (state, dispatch) = React.useReducer(createReducer, initialState);
  let { blinds, loading, deleting } = state;

  // Fetch the blinds on component mount.
  React.useEffect0(() => {
    Api.fetchBlinds(blinds => {
      dispatch(SetBlinds(blinds))
      dispatch(SetLoading(false));
    });

    let stopDelete = e => {
      if (ReactEvent.Keyboard.which(e) == 27) {
        dispatch(SetDeleting(false));
      }
    };

    Utils.add_keyboard_event_listener("keydown", stopDelete);

    Some(() => {
      Utils.remove_keyboard_event_listener("keydown", stopDelete);
    });
  });

  let onOrderEdited = (blind, newOrder) => {
    dispatch(SetLoading(true));

    Api.update(newOrder, response => {
      Js.log(response);
      dispatch(SetLoading(false));
      dispatch(EditOrderInBlind(blind, newOrder));
    });
  };

  let onOrderAdded = (blind, order) => {
    Api.add(blind.name, order, (response => {
      dispatch(AddOrderToBlind(blind, {
        ...order,
        id: response.newId,
      }));
    }));
  };

  let onRequestDelete = React.useCallback(() => {
    dispatch(SetDeleting(true));
  });

  let onOrderDelete = React.useCallback((blind, order) => {
    Api.delete(order, response => {
      Js.log(response.message);

      dispatch(RemoveOrderFromBlind(blind, order));
    });
  });

  <>
    {
      switch (loading) {
      | true => <div className="loading" />
      | false => React.null
      }
    }
    {React.array(
      blinds|>Array.of_list|>Array.mapi((index, blind) =>
        renderBlind(
          index,
          blind,
          onOrderEdited,
          onOrderAdded,
          onRequestDelete,
          onOrderDelete,
          deleting
        )
      )
    )}
  </>
};
