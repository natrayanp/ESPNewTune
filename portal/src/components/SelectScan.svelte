<script>
  import { createEventDispatcher } from 'svelte';
  const dispatch = createEventDispatcher();

  export let access_points;

</script>

 
    {#if access_points.length > 0}
      
        {#each access_points as ap}
          <div class="flex flex-row mb-2 min-w-full p-2 w-auto  shadow hover:shadow-xl hover:bg-green-100 rounded h-16 "  on:click={() => dispatch('select', ap)}>      
                  <div class="flex flex-col justify-center">
                    {#if ap.inlist}
                      <svg xmlns="http://www.w3.org/2000/svg" width="15" height="15" viewBox="0 0 448 512"><!--! Font Awesome Pro 6.1.1 by @fontawesome - https://fontawesome.com License - https://fontawesome.com/license (Commercial License) Copyright 2022 Fonticons, Inc. --><path d="M438.6 105.4C451.1 117.9 451.1 138.1 438.6 150.6L182.6 406.6C170.1 419.1 149.9 419.1 137.4 406.6L9.372 278.6C-3.124 266.1-3.124 245.9 9.372 233.4C21.87 220.9 42.13 220.9 54.63 233.4L159.1 338.7L393.4 105.4C405.9 92.88 426.1 92.88 438.6 105.4H438.6z"/></svg>
                    {/if}
                    <span class="w-3"></span>
                  </div>           
                  <div class="flex flex-col flex-grow justify-center">
                    <div class = "pl-5">
                      { ap.name }
                    </div>                    
                  </div>
                  <div class="flex flex-col justify-center">
                    {#if !ap.open}
                      <svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" style="fill: currentColor"><path d="M20,12c0-1.103-0.897-2-2-2h-1V7c0-2.757-2.243-5-5-5S7,4.243,7,7v3H6c-1.103,0-2,0.897-2,2v8c0,1.103,0.897,2,2,2h12 c1.103,0,2-0.897,2-2V12z M9,7c0-1.654,1.346-3,3-3s3,1.346,3,3v3H9V7z"></path></svg>
                    {/if}
                  </div>

          </div>
        {/each}
      
    {:else}
      <div class="flex flex-col rounded-xl items-center bg-red-100">
        <p class="mx-16 my-32">
          <svg xmlns="http://www.w3.org/2000/svg" width="48" height="48" viewBox="0 0 640 512"><!--! Font Awesome Pro 6.1.1 by @fontawesome - https://fontawesome.com License - https://fontawesome.com/license (Commercial License) Copyright 2022 Fonticons, Inc. --><path d="M319.1 351.1c-35.35 0-64 28.66-64 64.01s28.66 64.01 64 64.01c35.34 0 64-28.66 64-64.01S355.3 351.1 319.1 351.1zM320 191.1c-70.25 0-137.9 25.6-190.5 72.03C116.3 275.7 115 295.9 126.7 309.2C138.5 322.4 158.7 323.7 171.9 312C212.8 275.9 265.4 256 320 256s107.3 19.88 148.1 56C474.2 317.4 481.8 320 489.3 320c8.844 0 17.66-3.656 24-10.81C525 295.9 523.8 275.7 510.5 264C457.9 217.6 390.3 191.1 320 191.1zM630.2 156.7C546.3 76.28 436.2 32 320 32S93.69 76.28 9.844 156.7c-12.75 12.25-13.16 32.5-.9375 45.25c12.22 12.78 32.47 13.12 45.25 .9375C125.1 133.1 220.4 96 320 96s193.1 37.97 265.8 106.9C592.1 208.8 600 211.8 608 211.8c8.406 0 16.81-3.281 23.09-9.844C643.3 189.2 642.9 168.1 630.2 156.7z"/></svg>
          <!--svg xmlns="http://www.w3.org/2000/svg" width="48" height="48" viewBox="0 0 24 24" style="fill: currentColor"><path d="M17.671,14.307C16.184,12.819,14.17,12,12,12s-4.184,0.819-5.671,2.307l1.414,1.414c1.11-1.11,2.621-1.722,4.257-1.722 c1.636,0.001,3.147,0.612,4.257,1.722L17.671,14.307z"></path><circle cx="12" cy="18" r="2"></circle></svg-->
        </p>
        <p class="mb-10">
          No access points in range
        </p>
      </div>
    {/if}
  
    <!--
    <div class="text-center mt-5">      
      <button class="bg-blue-500 hover:bg-blue-700 text-white font-bold py-2 px-4 rounded-full" on:click={() => dispatch('refresh')}>
        Refresh
      </button>
    </div>
    -->
