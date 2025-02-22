/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

package org.chromium.chrome.browser.crypto_wallet.fragments.dapps;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;

import org.chromium.brave_wallet.mojom.AddSuggestTokenRequest;
import org.chromium.brave_wallet.mojom.CoinType;
import org.chromium.brave_wallet.mojom.NetworkInfo;
import org.chromium.brave_wallet.mojom.OriginInfo;
import org.chromium.chrome.R;
import org.chromium.chrome.browser.crypto_wallet.BlockchainRegistryFactory;
import org.chromium.chrome.browser.crypto_wallet.activities.BraveWalletBaseActivity;
import org.chromium.chrome.browser.crypto_wallet.util.Utils;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class AddTokenFragment extends BaseDAppsFragment {
    private AddSuggestTokenRequest mCurrentAddSuggestTokenRequest;
    private Button mBtCancel;
    private Button mBtAdd;
    private TextView mNetworkName;
    private TextView mWebSite;
    private TextView mTokenName;
    private TextView mTokenAddress;
    private ImageView mTokenImage;
    private ExecutorService mExecutor;
    private Handler mHandler;

    @Override
    public View onCreateView(
            LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        mExecutor = Executors.newSingleThreadExecutor();
        mHandler = new Handler(Looper.getMainLooper());
        View view = inflater.inflate(R.layout.fragment_add_token, container, false);

        mBtCancel = view.findViewById(R.id.fragment_add_btn_cancel);
        mBtAdd = view.findViewById(R.id.fragment_add_btn_add);
        mNetworkName = view.findViewById(R.id.fragment_add_token_tv_network_name);
        mWebSite = view.findViewById(R.id.fragment_add_token_tv_web_site);
        mTokenImage = view.findViewById(R.id.fragment_add_token_iv_token);
        mTokenName = view.findViewById(R.id.fragment_add_token_tv_token);
        mTokenAddress = view.findViewById(R.id.fragment_add_token_tv_address);
        initComponents(true);

        return view;
    }

    private void initComponents(boolean init) {
        fillAddSuggestTokenRequest(init);
        updateNetwork();
    }

    private void notifyAddSuggestTokenRequestProcessed(boolean approved) {
        String[] addresses = new String[] {mCurrentAddSuggestTokenRequest.token.contractAddress};
        getBraveWalletService().notifyAddSuggestTokenRequestsProcessed(approved, addresses);
        initComponents(false);
    }

    private void fillAddSuggestTokenRequest(boolean init) {
        getBraveWalletService().getPendingAddSuggestTokenRequests(requests -> {
            if (requests == null || requests.length == 0) {
                Intent intent = new Intent();
                getActivity().setResult(Activity.RESULT_OK, intent);
                getActivity().finish();

                return;
            }
            mCurrentAddSuggestTokenRequest = requests[0];
            if (init) {
                mBtCancel.setOnClickListener(
                        v -> { notifyAddSuggestTokenRequestProcessed(false); });
                mBtAdd.setOnClickListener(v -> { notifyAddSuggestTokenRequestProcessed(true); });
                mTokenAddress.setOnClickListener(v -> {
                    Activity activity = getActivity();
                    if (activity instanceof BraveWalletBaseActivity) {
                        Utils.openAddress(
                                "/token/" + mCurrentAddSuggestTokenRequest.token.contractAddress,
                                getJsonRpcService(), (BraveWalletBaseActivity) activity);
                    }
                });
            }
            mWebSite.setText(mCurrentAddSuggestTokenRequest.origin.originSpec);
            initToken();
        });
    }

    private void updateNetwork() {
        getJsonRpcService().getChainId(CoinType.ETH, chainId -> {
            getJsonRpcService().getAllNetworks(CoinType.ETH, chains -> {
                NetworkInfo[] customNetworks = Utils.getCustomNetworks(chains);
                mNetworkName.setText(
                        Utils.getNetworkText(getActivity(), chainId, customNetworks).toString());
            });
        });
    }

    private void initToken() {
        if (!mCurrentAddSuggestTokenRequest.token.logo.isEmpty()) {
            String tokensPath = BlockchainRegistryFactory.getInstance().getTokensIconsLocation();
            Utils.setBitmapResource(mExecutor, mHandler, getActivity(),
                    "file://" + tokensPath + "/" + mCurrentAddSuggestTokenRequest.token.logo,
                    R.drawable.ic_eth, mTokenImage, null, true);
        } else {
            Utils.setBlockiesBitmapCustomAsset(mExecutor, mHandler, mTokenImage,
                    mCurrentAddSuggestTokenRequest.token.contractAddress,
                    mCurrentAddSuggestTokenRequest.token.symbol,
                    getActivity().getResources().getDisplayMetrics().density, null, getActivity(),
                    false, (float) 0.9);
        }
        String tokenName = mCurrentAddSuggestTokenRequest.token.name;
        if (tokenName.isEmpty()) {
            tokenName = mCurrentAddSuggestTokenRequest.token.symbol;
        } else {
            tokenName += " (" + mCurrentAddSuggestTokenRequest.token.symbol + ")";
        }
        mTokenName.setText(tokenName);
        mTokenAddress.setText(
                Utils.stripAccountAddress(mCurrentAddSuggestTokenRequest.token.contractAddress));
    }
}
