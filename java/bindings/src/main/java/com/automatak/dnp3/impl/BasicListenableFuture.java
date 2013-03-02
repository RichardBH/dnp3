/**
 * Copyright 2013 Automatak, LLC
 *
 * Licensed to Automatak, LLC (www.automatak.com) under one or more
 * contributor license agreements. See the NOTICE file distributed with this
 * work for additional information regarding copyright ownership. Automatak, LLC
 * licenses this file to you under the GNU Affero General Public License
 * Version 3.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 * http://www.gnu.org/licenses/agpl.html
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */

package com.automatak.dnp3.impl;


import com.automatak.dnp3.ListenableFuture;

import java.util.LinkedList;
import java.util.List;

class BasicListenableFuture<T> implements ListenableFuture<T>, Promise<T> {

    private final Object mutex = new Object();
    private final List<CompletionListener<T>> listeners = new LinkedList<CompletionListener<T>>();
    private T value = null;

    public T get()
    {
        synchronized (mutex)
        {
            while(value == null) {
                try {
                    mutex.wait();
                }
                catch(InterruptedException ex) {
                    throw new RuntimeException(ex);
                }
            }
            return value;
        }
    }

    public void addListener(CompletionListener<T> listener)
    {
       if(listener == null) throw new IllegalArgumentException("listener cannot be null");
       synchronized (mutex)
       {
           if(value == null) listeners.add(listener);
           else listener.onComplete(value);
       }
    }

    public void set(T value)
    {
        if(value == null) throw new IllegalArgumentException("value cannot be null");
        synchronized (mutex)
        {
            if(this.value == null) {
                this.value = value;
                mutex.notifyAll();
                for(CompletionListener<T> l: listeners) l.onComplete(value);
                listeners.clear();
            }
            else throw new IllegalStateException("value has already been set");
        }
    }

}
