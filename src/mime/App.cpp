/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024-2026 SEN Labs e.U.
 */

#include <errno.h>
#include <kernel/fs_index.h>
#include <Directory.h>
#include <Entry.h>
#include <FindDirectory.h>
#include <Messenger.h>
#include <MimeType.h>
#include <Node.h>
#include <Path.h>
#include <Resources.h>
#include <stdio.h>
#include <string.h>
#include <String.h>
#include <Volume.h>
#include <VolumeRoster.h>

#include <sen/Sen.h>
#include <sen/SenOntoCore.h>

status_t InstallMimeTypeFromResource(const char* path, BString* installedType);
status_t RelateToMimeType(const char* file, const char* mimeType, const char* label, bool remove = false);
status_t DeleteMimeType(const char* mimeType);
status_t GetInstalledMimeTypes(const char* supertype, BMessage* types);
void PrintUsage(const char* name);
void CreateIndexOnAllVolumes(const char* attrName, uint32 attrType);


int
main(int argc, char** argv)
{
    if (argc == 1) {
        PrintUsage(argv[0]);
        return EXIT_FAILURE;
    }
    status_t result;
    const char* command = argv[1];
    if (strncmp(command, "install", strlen("install")) == 0) {
        BString installedType;
        result = InstallMimeTypeFromResource(argv[2], &installedType);
        if (result != B_OK) {
            fprintf(stderr, "failed to install MIME type from %s: %s\n", argv[2], strerror(result));
        } else {
            // the installer script reads the type from this line
            printf("successfully installed MIME type %s.\n", installedType.String());
        }
    }
    else if (strcmp(command, "unrelate") == 0) {
        if (argc < 4) {
            PrintUsage(argv[0]);
            return EXIT_FAILURE;
        }
        result = RelateToMimeType(argv[2], argv[3], "provides", true);
        if (result != B_OK)
            fprintf(stderr, "failed to remove the relation of %s to MIME type %s: %s\n", argv[2], argv[3], strerror(result));
    }
    else if (strcmp(command, "relate") == 0) {
        if (argc < 4) {
            PrintUsage(argv[0]);
            return EXIT_FAILURE;
        }
        result = RelateToMimeType(argv[2], argv[3], argc > 4 ? argv[4] : "provides");
        if (result != B_OK) {
            fprintf(stderr, "failed to relate %s to MIME type %s: %s\n", argv[2], argv[3], strerror(result));
        } else {
            printf("%s %s %s.\n", argv[2], argc > 4 ? argv[4] : "provides", argv[3]);
        }
    }
    else if (strncmp(command, "uninstall", strlen("uninstall")) == 0) {
        result = DeleteMimeType(argv[2]);
        if (result != B_OK) {
            fprintf(stderr, "failed to uninstall MIME type %s: %s\n", argv[2], strerror(result));
        } else {
            printf("successfully uninstalled MIME type %s.\n", argv[2]);
        }
    }
    else if (strncmp(command, "list", strlen("list")) == 0) {
        BMessage entityTypes;
        result = GetInstalledMimeTypes("entity", &entityTypes);
        if (result != B_OK) {
            fprintf(stderr, "failed to query MIME type DB: %s\n", strerror(result));
        }
        printf("installed entities:\n");
        entityTypes.PrintToStream();

        BMessage relationTypes;
        result = GetInstalledMimeTypes("relation", &relationTypes);
        if (result != B_OK) {
            fprintf(stderr, "failed to query MIME type DB: %s\n", strerror(result));
        }
        printf("installed relations:\n");
        relationTypes.PrintToStream();
    }
    else {
        fprintf(stderr, "unknown command %s\n", command);
        return EXIT_FAILURE;
    }

	return result == B_OK ? EXIT_SUCCESS : EXIT_FAILURE;
}

void PrintUsage(const char* progname) {
    BPath path(progname);

    printf("Usage: %s <operation> [mime-type]\n", path.Leaf());
    printf("where operation is one of:\n\n");

    printf("install     installs MIME type in MIME db\n");
    printf("relate      <file> <mime-type> [label]: relates the file to the installed MIME type (label: provides), see SEN server\n");
    printf("unrelate    <file> <mime-type>: removes that relation again\n");
    printf("uninstall   uninstalls MIME type from MIME db\n");
    printf("list        lists entities and relations in MIME db\n");

    return;
}

status_t InstallMimeTypeFromResource(const char* path, BString* installedType) {
    BResources resources(path);
    if (! BFile(path, B_READ_ONLY).IsReadable()) {
        fprintf(stderr, "cannot read resources from path %s: check path is valid!\n", path);
        return B_ERROR;
    }

    status_t result = resources.InitCheck();
    if (result != B_OK) {
        fprintf(stderr, "error initializing resources from path %s: %s\n", path, strerror(result));
        return result;
    }

    BMimeType mimeType;
    BMessage message;
    const void *type, *sDesc, *lDesc, *attrInfo, *senConfig, *extens, *snifferRule, *prefApp, *icon;
    size_t* size = new size_t;

    // get Type
    type = resources.LoadResource(B_STRING_TYPE, "META:TYPE", size);
    if (type == NULL) {
        return B_ERROR;
    }

    const char* mime = reinterpret_cast<const char*>(type);
    *installedType = mime;
    mimeType.SetTo(mime);
    if (!mimeType.IsValid()) {
        fprintf(stderr, "invalid MIME type '%s' in resource %s: %s\n", mime, path, strerror(result));
        return result;
    }
    if (mimeType.IsInstalled()) {
        printf("MIME type %s is already installed, updating...\n", mime);
    } else {
        // we need to install as first step, since all other MimeType operations act on the MIME DB directly:(
        result = mimeType.Install();
        if (result != B_OK) {
            fprintf(stderr, "error installing MIME type %s from resource %s: %s\n", mime, path, strerror(result));
            return result;
        }
    }

    // get short description (used as type name in prefs)
    sDesc = resources.LoadResource('MSDC', "META:S:DESC", size);
    if (sDesc == NULL) {
        return B_ERROR;
    }
    mimeType.SetShortDescription(reinterpret_cast<const char*>(sDesc));

    // get long description (optional)
    lDesc = resources.LoadResource('MLDC', "META:L:DESC", size);
    if (lDesc != NULL) {
        mimeType.SetLongDescription(reinterpret_cast<const char*>(lDesc));
    }

    // get preferred app
    prefApp = resources.LoadResource('MSIG', "META:PREF_APP", size);
    if (prefApp != NULL) {
        mimeType.SetPreferredApp(reinterpret_cast<const char*>(prefApp));
    }

    // get sniffer rule
    snifferRule = resources.LoadResource(B_STRING_TYPE, "META:SNIFF_RULE", size);
    if (snifferRule != NULL) {
        mimeType.SetSnifferRule(reinterpret_cast<const char*>(snifferRule));
    }

    // get extensions
    extens = resources.LoadResource(B_MESSAGE_TYPE, "META:EXTENS", size);
    if (extens != NULL && message.Unflatten(reinterpret_cast<const char*>(extens)) == B_OK) {
        mimeType.SetFileExtensions(&message);
    }

    // get attribute info
    attrInfo = resources.LoadResource(B_MESSAGE_TYPE, "META:ATTR_INFO", size);
    if (attrInfo != NULL && message.Unflatten(reinterpret_cast<const char*>(attrInfo)) == B_OK) {
        mimeType.SetAttrInfo(&message);

        // create the indices of the attributes that are marked searchable, on every mounted volume that supports them.
        // An index is never removed here: attribute names are shared with other programs (dc:title, ...).
        int32 attrCount = 0;
        message.GetInfo("attr:name", NULL, &attrCount);

        for (int32 i = 0; i < attrCount; i++) {
            if (! message.GetBool("attr:searchable", i, false))
                continue;

            const char* attrName = message.GetString("attr:name", i, "");
            uint32      attrType = (uint32) message.GetInt32("attr:type", i, B_STRING_TYPE);   // stored as int32 by rdef

            CreateIndexOnAllVolumes(attrName, attrType);
        }
    }

    // write optional SEN configuration in its own attribute, so FileTypes
    // prefs won't override it! (also, it doesn't belong into ATTR_INFO)
    // since the MimeType API doesn't support custom data, we need to write
    // into the filesystem's MIME DB directly.
    senConfig = resources.LoadResource(B_MESSAGE_TYPE, sen::attr::kRelationConfig, size);
    if (senConfig != NULL && message.Unflatten(reinterpret_cast<const char*>(senConfig)) == B_OK) {
        BPath path;
        result = find_directory(B_USER_SETTINGS_DIRECTORY, &path);
        if (result != B_OK) {
            fprintf(stderr, "could not find user settings directory: %s\n", strerror(result));
            return result;
        }

        path.Append("mime_db");

        // read or create and init settings file
        BDirectory mimeDir(path.Path());
        // the newly created MIME type is reflected in the file system like a path supertype/subtype
        BEntry mimeEntry(&mimeDir, mimeType.Type());

        result = mimeEntry.InitCheck();
        if (result != B_OK) {
            if (! mimeEntry.Exists()) {
                fprintf(stderr, "could not find MIME type %s in MIME DB fs: %s\n", mimeType.Type(), strerror(result));
            } else {
                fprintf(stderr, "error accessing MIME type %s in MIME DB fs: %s\n", mimeType.Type(), strerror(result));
            }
            return result;
        }

        // write config to MIME type file directly
        BNode mimeNode(&mimeEntry);
        if (mimeNode.InitCheck() != B_OK) {
            fprintf(stderr, "error accessing MIME DB file %s: %s\n", mimeType.Type(), strerror(result));
            return result;
        }

        size_t sizeResult = mimeNode.WriteAttr(sen::attr::kRelationConfig, B_MESSAGE_TYPE, 0, senConfig, *size);
        if (sizeResult < *size) {
            if (sizeResult < 0)
                result = sizeResult;
            else
                result = B_ERROR;

            fprintf(stderr, "error writing SEN:CONFIG to MIME DB file %s: %s\n", mimeType.Type(), strerror(result));
            return result;
        }

        // write additional optional collection icon (e.g. for relation folders)
        icon = resources.LoadResource(B_VECTOR_ICON_TYPE, sen::attr::kRelationFolderIcon, size);
        if (icon != NULL && *size > 0) {
            size_t sizeResult = mimeNode.WriteAttr(sen::attr::kRelationFolderIcon, B_VECTOR_ICON_TYPE, 0, icon, *size);
            if (sizeResult < *size) {
                if (sizeResult < 0)
                    result = sizeResult;
                else
                    result = B_ERROR;

                fprintf(stderr, "error writing SEN:ICON:FOLDER to MIME DB file %s: %s\n", mimeType.Type(), strerror(result));
                return result;
            }
        }
    }

    // lastly, add optional icon
    icon = resources.LoadResource(B_VECTOR_ICON_TYPE, "META:ICON", size);
    if (icon != NULL && *size > 0) {
        mimeType.SetIcon(reinterpret_cast<const uint8*>(icon), *size);
    }

    return B_OK;
}

status_t DeleteMimeType(const char* type) {
    BMimeType mimeType(type);
    status_t result;

    if (!mimeType.IsValid()) {
        fprintf(stderr, "%s is not a valid MIME type.\n", type);
        return result;
    }
    if (!mimeType.IsInstalled()) {
        printf("MIME type %s is not installed, skipping...", type);
    }

    return mimeType.Delete();
}

status_t
GetInstalledMimeTypes(const char* supertype, BMessage* types)
{
	return BMimeType::GetInstalledTypes(supertype, types);
}

/**
 * @brief Create the index for an attribute on every mounted volume that can have one (BFS, queries and attributes supported).
 *
 * @param attrName the attribute to index
 * @param attrType the type code of the attribute (an index has the type of its attribute)
 */
void
CreateIndexOnAllVolumes(const char* attrName, uint32 attrType)
{
    BVolumeRoster roster;
    BVolume volume;

    while (roster.GetNextVolume(&volume) == B_OK) {
        if (! volume.KnowsQuery() || ! volume.KnowsAttr() || volume.IsReadOnly())
            continue;

        char volumeName[B_FILE_NAME_LENGTH];
        volume.GetName(volumeName);

        if (fs_create_index(volume.Device(), attrName, attrType, 0) == 0) {
            printf("* created index for '%s' on volume %s\n", attrName, volumeName);
        } else if (errno == B_FILE_EXISTS) {
            printf("* index for '%s' exists on volume %s, skipping.\n", attrName, volumeName);
        } else {
            fprintf(stderr, "failed to create index for '%s' on volume %s: %s\n", attrName, volumeName, strerror(errno));
        }
    }
}


/** Relate a file (e.g. the placeholder of an ontology) to the file of an installed MIME type in the MIME database, with a generic
 *  reference of the SEN server. Relating again is not an error: the server tells that the relation exists (409). */
status_t RelateToMimeType(const char* file, const char* mimeType, const char* label, bool remove) {
    BMimeType type(mimeType);
    if (!type.IsValid() || !type.IsInstalled()) {
        fprintf(stderr, "MIME type %s is not installed.\n", mimeType);
        return B_ENTRY_NOT_FOUND;
    }

    // the MIME database keeps every type as a file
    BPath path;
    status_t result = find_directory(B_USER_SETTINGS_DIRECTORY, &path);
    if (result == B_OK)
        result = path.Append("mime_db");
    if (result == B_OK)
        result = path.Append(mimeType);

    entry_ref source, target;
    if (result == B_OK)
        result = get_ref_for_path(file, &source);
    if (result == B_OK)
        result = get_ref_for_path(path.Path(), &target);
    if (result != B_OK)
        return result;

    BMessenger server(sen::kServerSignature);
    if (!server.IsValid()) {
        fprintf(stderr, "the SEN server is not running, run this again when it is.\n");
        return B_ERROR;
    }

    // The relations of an ontology to its types are read-only: users cannot change the configuration by mistake. The installer
    // does, with the override, so a relation that an older installer made (not read-only) is replaced, not added to.
    for (int step = 0; step < (remove ? 1 : 2); step++) {
        bool removing = remove || step == 0;

        BMessage message(removing ? sen::cmd::kRelationRemove : sen::cmd::kRelationAdd);
        message.AddRef(sen::key::kSourceRef, &source);
        message.AddString(sen::key::kRelationType, sen::onto::core::mime::kReference);
        message.AddRef(sen::key::kTargetRef, &target);
        if (removing) {
            message.AddBool(sen::key::kOverride, true);
            message.AddBool(sen::key::kAllRelations, true);
        } else {
            BMessage properties;
            properties.AddString(sen::attr::kRelationLabel, label);
            properties.AddBool(sen::attr::kRelationReadOnly, true);
            message.AddMessage(sen::key::kRelationProperties, &properties);

            // seen from the type, the ontology provides it: the opposite direction is "provided by"
            BMessage inverse;
            inverse.AddString(sen::attr::kRelationLabel, "provided by");
            message.AddMessage(sen::key::kInverseProperties, &inverse);
        }

        BMessage reply;
        result = server.SendMessage(&message, &reply, 10000000, 10000000);
        if (result != B_OK)
            return result;

        int32 status = reply.GetInt32(sen::key::kStatus, -1);
        if (removing && !remove)
            continue;   // the old one is gone, or there was none
        if (status == sen::status::kErrConflict || (remove && status == sen::status::kErrRelationNotFound)
                || (remove && status == sen::status::kErrNotFound))
            return B_OK;    // exists already, or is not there (any more)
        if (status < 200 || status >= 300) {
            fprintf(stderr, "the SEN server answered %d: %s\n", (int) status, reply.GetString(sen::key::kDetail, ""));
            return B_ERROR;
        }
    }
    return B_OK;
}
