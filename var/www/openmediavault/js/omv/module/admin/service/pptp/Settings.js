/**
 *
 * @license   http://www.gnu.org/licenses/gpl.html GPL Version 3
 * Copyright (C) 2013 OpenMediaVault Plugin Developers
 *
 * OpenMediaVault is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * OpenMediaVault is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenMediaVault. If not, see <http://www.gnu.org/licenses/>.
 */
// require("js/omv/WorkspaceManager.js")
// require("js/omv/workspace/form/Panel.js")

Ext.define("OMV.module.admin.service.pptp.Settings", {
    extend: "OMV.workspace.form.Panel",

    rpcService: "Pptp",
    rpcGetMethod: "getSettings",
    rpcSetMethod: "setSettings",

    getFormItems: function() {
        var me = this;
        return [{
            xtype: "fieldset",
            title: _("General settings"),
            fieldDefaults: {
                labelSeparator: ""
            },
            items: [{
                xtype: "checkbox",
                name: "enable",
                fieldLabel: _("Enable"),
                checked: false
            },{
                xtype: "textfield",
                name: "localip",
                fieldLabel: _("Local IP"),
                allowBlank: false,
                plugins: [{
                    ptype: "fieldinfo",
                    text: _("IP address of this server - example: 192.168.1.99")
                }]
            },{
                xtype: "textfield",
                name: "remoteip",
                fieldLabel: _("Remote IP"),
                value: "",
                plugins: [{
                    ptype: "fieldinfo",
                    text: _("These are the address/addresses that will be handed out to the clients. The following scheme is mandatory:") +
                            "<ul>" +
                            "<li>" + _("define a single IP - example: 192.168.1.45") + "</li>" +
                            "<li>" + _("define a range of IPs - example: 192.168.1.24-28") + "</li>" +
                            "<li>" + _("define a multiple IPs - example: 192.168.1.13,192.168.1.21-25,192.168.1.78") + "</li>" +
                            "</ul>"
                }]
            },{
                xtype: "textfield",
                name: "msdns",
                fieldLabel: _("DNS Server"),
                allowBlank: false,
                plugins: [{
                    ptype: "fieldinfo",
                    text: _("The DNS server for the local network your client will be connecting to - example: 192.168.1.1")
                }]
            }]
        }];
    }
});

OMV.WorkspaceManager.registerPanel({
    id: "settings",
    path: "/service/pptp",
    text: _("Settings"),
    position: 10,
    className: "OMV.module.admin.service.pptp.Settings"
});
