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
// require("js/omv/workspace/grid/Panel.js")
// require("js/omv/workspace/window/Form.js")
// require("js/omv/workspace/window/plugin/ConfigObject.js")
// require("js/omv/Rpc.js")
// require("js/omv/data/Store.js")
// require("js/omv/data/Model.js")
// require("js/omv/data/proxy/Rpc.js")

/**
 * @class OMV.module.admin.service.pptp.User
 * @derived OMV.workspace.window.Form
 */
Ext.define("OMV.module.admin.service.pptp.User", {
    extend : "OMV.workspace.window.Form",
    uses   : [
        "OMV.workspace.window.plugin.ConfigObject"
    ],

    rpcService   : "Pptp",
    rpcGetMethod : "getUser",
    rpcSetMethod : "setUser",
    plugins      : [{
        ptype : "configobject"
    }],

    getFormItems : function () {
        var me = this;
        return [{
            xtype: "textfield",
            name: "username",
            fieldLabel: _("Username"),
            allowBlank: false,
        },{
            xtype: "textfield",
            name: "password",
            fieldLabel: _("Password"),
            allowBlank: false,
        }];
    }
});

/**
 * @class OMV.module.admin.service.pptp.Users
 * @derived OMV.workspace.grid.Panel
 */
Ext.define("OMV.module.admin.service.pptp.Users", {
    extend   : "OMV.workspace.grid.Panel",
    requires : [
        "OMV.Rpc",
        "OMV.data.Store",
        "OMV.data.Model",
        "OMV.data.proxy.Rpc"
    ],
    uses     : [
        "OMV.module.admin.service.pptp.User"
    ],

    hidePagingToolbar : false,
    stateful          : true,
    stateId           : "9889057b-b2c0-4c48-a4c1-8c9b4fb54d7b",
    columns           : [{
        text      : _("Username"),
        sortable  : true,
        dataIndex : "username",
        stateId   : "username"
    }],

    initComponent : function () {
        var me = this;
        Ext.apply(me, {
            store : Ext.create("OMV.data.Store", {
                autoLoad : true,
                model    : OMV.data.Model.createImplicit({
                    idProperty : "uuid",
                    fields     : [
                        { name  : "uuid", type: "string" },
                        { name  : "username", type: "string" }
                    ]
                }),
                proxy    : {
                    type    : "rpc",
                    rpcData : {
                        service : "Pptp",
                        method  : "getUserList"
                    }
                }
            })
        });
        me.callParent(arguments);
    },

    onAddButton: function () {
        var me = this;
        Ext.create("OMV.module.admin.service.pptp.User", {
            title     : _("Add user"),
            uuid      : OMV.UUID_UNDEFINED,
            listeners : {
                scope  : me,
                submit : function () {
                    this.doReload();
                }
            }
        }).show();
    },

    onEditButton: function () {
        var me, record;

        me = this;
        record = me.getSelected();

        Ext.create("OMV.module.admin.service.pptp.User", {
            title     : _("Edit user"),
            uuid      : record.get("uuid"),
            listeners : {
                scope  : me,
                submit : function () {
                    this.doReload();
                }
            }
        }).show();
    },

    doDeletion: function (record) {
        var me = this;
        OMV.Rpc.request({
            scope    : me,
            callback : me.onDeletion,
            rpcData  : {
                service : "Pptp",
                method  : "deleteUser",
                params  : {
                    uuid: record.get("uuid")
                }
            }
        });
    }
});

OMV.WorkspaceManager.registerPanel({
    id        : "Users",
    path      : "/service/pptp",
    text      : _("Users"),
    position  : 20,
    className : "OMV.module.admin.service.pptp.Users"
});
