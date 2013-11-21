<?php
/**
 *
 * @license   http://www.gnu.org/licenses/gpl.html GPL Version 3
 * @author    Marcel Beck <marcel.beck@mbeck.org>
 * @copyright Copyright (c) 2012 Marcel Beck
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
require_once("openmediavault/object.inc");
require_once("openmediavault/config.inc");
require_once("openmediavault/error.inc");
require_once("openmediavault/util.inc");
require_once("openmediavault/rpcservice.inc");
require_once("openmediavault/notify.inc");

class OMVRpcServicePptp extends OMVRpcServiceAbstract {

    public function getName() {
        return "Pptp";
    }

    public function initialize() {
        $this->registerMethod("getSettings");
        $this->registerMethod("setSettings");
        $this->registerMethod("getUserList");
        $this->registerMethod("getUser");
        $this->registerMethod("setUser");
        $this->registerMethod("deleteUser");
    }

    public function getSettings($params, $context) {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext($context, array( "role" => OMV_ROLE_ADMINISTRATOR ));

        // Get the configuration object.
        $xpath = "//services/pptp";
        $object = $xmlConfig->get($xpath);
        if(is_null($object)) {
            throw new OMVException(OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED, $xpath);
        }

        // Modify the result object.
        $object['enable'] = boolval($object['enable']);

        // Remove useless fields from the result object.
        unset($object['users']);
        return $object;
    }

    public function setSettings($params, $context) {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext($context, array( "role" => OMV_ROLE_ADMINISTRATOR ));

        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, '{
            "type"       : "object",
            "properties" : {
                "enable"   : { "type" : "boolean" },
                "localip"  : { "type" : "string" },
                "remoteip" : { "type" : "string" },
                "msdns"    : { "type" : "string" }
            }
        }');

        // Get the existing configuration object.
        $xpath = "//services/pptp";
        $oldObject = $xmlConfig->get($xpath);
        if(is_null($oldObject)) {
            throw new OMVException(OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED, $xpath);
        }

        // Update the configuration object.
        $object = array(
            "enable"   => array_boolval($params, 'enable'),
            "localip"  => $params['name'],
            "remoteip" => $params['port'],
            "msdns"    => $params['port'],
            "users"    => isset($oldObject['users']) ? $oldObject['users'] : array(),
        );
        if(FALSE === $xmlConfig->replace("//services/pptp", $object)) {
            throw new OMVException(OMVErrorMsg::E_CONFIG_SET_OBJECT_FAILED);
        }

        // Notify configuration changes.
        $dispatcher = &OMVNotifyDispatcher::getInstance();
        $dispatcher->notify(OMV_NOTIFY_MODIFY,
            "org.openmediavault.services.pptp", $object);

        // Return the configuration object.
        return $object;
    }

    public function getUserList($params, $context) {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext($context, array( "role" => OMV_ROLE_ADMINISTRATOR ));

        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, '{
            "type"       : "object",
            "properties" : {
                "start"     : { "type" : "integer" },
                "limit"     : { '.$GLOBALS['OMV_JSONSCHEMA_COUNTFIELD'].' },
                "sortfield" : { '.$GLOBALS['OMV_JSONSCHEMA_SORTFIELD'].' },
                "sortdir"   : { '.$GLOBALS['OMV_JSONSCHEMA_SORTDIR'].' }
            }
        }');

        // Get the configuration object.
        $xpath = "//services/pptp/users/user";
        $objects = $xmlConfig->getList($xpath);
        if(is_null($objects)) {
            throw new OMVException(OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED, $xpath);
        }

        // Filter the result.
        return $this->applyFilter($objects, $params['start'], $params['limit'],
            $params['sortfield'], $params['sortdir']);
    }

    public function getUser($params, $context) {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext($context, array( "role" => OMV_ROLE_ADMINISTRATOR ));

        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, '{
            "type"       : "object",
            "properties" : {
                "uuid" : { '.$GLOBALS['OMV_JSONSCHEMA_UUID'].' }
            }
        }');

        // Get the configuration object.
        $xpath = sprintf("//services/pptp/users/user[uuid='%s']", $params['uuid']);
        $object = $xmlConfig->get($xpath);
        if(is_null($object)) {
            throw new OMVException(OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED, $xpath);
        }

        return $object;
    }

    public function setUser($params, $context) {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext($context, array( "role" => OMV_ROLE_ADMINISTRATOR ));

        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, '{
            "type"       : "object",
            "properties" : {
                "uuid"     : { '.$GLOBALS['OMV_JSONSCHEMA_UUID_UNDEFINED'].' },
                "username" : { "type" : "string" }
                "password" : { "type" : "string" }
            }
        }');

        // Prepare the configuration object.
        $object = array(
            "uuid"     => ($params['uuid'] == $GLOBALS['OMV_UUID_UNDEFINED']) ? OMVUtil::uuid() : $params['uuid'],
            "username" => $params['username'],
            "password" => $params['password']
        );

        // Set the configuration object.
        $success = FALSE;
        if($params['uuid'] == $GLOBALS['OMV_UUID_UNDEFINED']) {
            // Check uniqueness. A user should only appear in the list once.
            if ( TRUE === $xmlConfig->exists("//services/pptp/users/user[username='{$params['username']}']") ) {
                throw new OMVException(OMVErrorMsg::E_CONFIG_OBJECT_UNIQUENESS,
                  gettext("That username is already in the list!"));
            }

            // Append the configuration object.
            $success = $xmlConfig->set("//services/pptp/users", array("user" => $object));
        } else {
            // Update the existing configuration object.
            $success = $xmlConfig->replace("//services/pptp/users/user[uuid='{$params['uuid']}']", $object);
        }
        if(FALSE === $success) {
            throw new OMVException(OMVErrorMsg::E_CONFIG_SET_OBJECT_FAILED);
        }

        // Notify configuration changes.
        $dispatcher = &OMVNotifyDispatcher::getInstance();
        $dispatcher->notify(($params['uuid'] == $GLOBALS['OMV_UUID_UNDEFINED']) ?
            OMV_NOTIFY_CREATE : OMV_NOTIFY_MODIFY,
            "org.openmediavault.services.pptp.users.user", $object);

        // Return the configuration object.
        return $object;
    }

    public function deleteUser($params, $context) {
        // Validate the RPC caller context.
        $this->validateMethodContext($context, array( "role" => OMV_ROLE_ADMINISTRATOR ));

        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, '{
            "type"       : "object",
            "properties" : {
                "uuid" : { '.$GLOBALS['OMV_JSONSCHEMA_UUID'].' }
            }
        }');

        // Delete the configuration object.
        $xpath = sprintf("//services/pptp/users/user[uuid='%s']", $params['uuid']);
        $object = $this->deleteConfigObjectByPath($xpath, "org.openmediavault.services.pptp.users.user");

        // Return the deleted configuration object.
        return $object;
    }
}

// Register the RPC service.
$rpcServiceMgr = &OMVRpcServiceMgr::getInstance();
$rpcServiceMgr->registerService(new OMVRpcServicePptp());
?>