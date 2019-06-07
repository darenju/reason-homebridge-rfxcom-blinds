open Utils;

type state = {
  addOrder: bool,
};

type action =
  | SetAddOrder(bool);

let createReducer = (_, action) =>
  switch (action) {
  | SetAddOrder(status) => { addOrder: status }
  };

let renderOrders = (
  orders: list(order),
  openCloseSeconds,
  onOrderEdit,
  onRequestDelete,
  onDelete,
  deleting,
) =>
  React.array(Array.of_list(orders) |> Array.map(order =>
    <Order
      key=("order-" ++ order.id)
      openCloseSeconds
      order
      onOrderEdit
      onRequestDelete
      onDelete
      deleting
    />
  ));

[@react.component]
let make = (
  ~blind: blind,
  ~onOrderEdit,
  ~onOrderAdd,
  ~onRequestDelete,
  ~onOrderDelete,
  ~deleting: bool,
) => {
  let {
    name,
    openCloseSeconds,
    orders,
  } = blind;

  let (state, dispatch) = React.useReducer(createReducer, {
    addOrder: false,
  });

  let { addOrder } = state;

  let onOrderEdited = React.useCallback(newOrder => {
    onOrderEdit(blind, newOrder);
  });

  let onToggleAddOrder = React.useCallback(e => {
    e|>ignore;

    dispatch(SetAddOrder(!addOrder));
  });

  let onOrderAdded = React.useCallback(newOrder => {
    dispatch(SetAddOrder(false));
    onOrderAdd(blind, newOrder);
  });

  let onOrderDeleted = React.useCallback(orderToDelete => {
    onOrderDelete(blind, orderToDelete);
  });

  <div className="blind">
    {name |> ReasonReact.string}

    <div className="blinds">
      {
        renderOrders(
          orders,
          openCloseSeconds,
          onOrderEdited,
          onRequestDelete,
          onOrderDeleted,
          deleting,
        )
      }

      {
        switch (addOrder) {
        | true =>
          <Order
            order=({ id: "", time: "", duration: None, direction: "up" })
            openCloseSeconds
            onOrderEdit=(onOrderAdded)
            onRequestDelete=(() => ())
            deleting=(false)
            onCancel=(() => dispatch(SetAddOrder(false)))
            onDelete=(_ => ())
            isNew=(true)
          />
        | false => React.null
        }
      }

      <div
        onClick=(onToggleAddOrder)
        className=("order order__add" ++ (deleting ? " order--invisible" : ""))
      >
        <em className=("icon icon-add order__icon" ++ (addOrder ? " order__icon--flip" : "")) />
      </div>
    </div>
  </div>
};
