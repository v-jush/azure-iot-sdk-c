# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE file in the project root for full license information.


git fetch origin 
[ $? -eq 0 ] || { echo "git fetch failed"; exit 1; }
git checkout $CLIENTLIBRARY_COMMIT_SHA
[ $? -eq 0 ] || { echo "git checkout failed"; exit 1; }
git submodule update --init --recursive
[ $? -eq 0 ] || { echo "git submodule failed"; exit 1; }
