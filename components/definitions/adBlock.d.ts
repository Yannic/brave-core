declare namespace AdBlock {
  export interface ApplicationState {
    adblockData: State | undefined
  }

  export interface State {
    settings: {
      customFilters: string
      regionalLists: FilterList[]
      listSubscriptions: SubscriptionInfo[]
    }
  }

  export interface FilterList {
    uuid: string
    url: string
    title: string
    supportUrl: string
    componentId: string
    base64PublicKey: string
    enabled: boolean
  }

  export interface SubscriptionInfo {
    list_url: string
    title: string
    last_update_was_successful: boolean
    last_update_attempt: number
    enabled: boolean
  }
}
